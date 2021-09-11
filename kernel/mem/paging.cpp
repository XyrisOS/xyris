/**
 * @file paging.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-22
 *
 * @copyright Copyright Keeton Feavel and Micah Switzer (c) 2019
 *
 */
#include <arch/arch.hpp>
#include <boot/Arguments.hpp>
#include <dev/serial/rs232.hpp>
#include <lib/bitset.hpp>
#include <lib/mutex.hpp>
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <mem/paging.hpp>
#include <meta/sections.hpp>
#include <stddef.h>

#define PAGE_ENTRIES        1024
#define ADDRESS_SPACE_SIZE  0x100000000
#define KADDR_TO_PHYS(addr) ((addr) - KERNEL_BASE)
#define ADDR(addr)          ((union address) { .val = (addr) })

static uint32_t machine_page_count;
static Mutex mutex_paging("paging");

#define MEM_BITMAP_SIZE ((ADDRESS_SPACE_SIZE / PAGE_SIZE) / (sizeof(size_t) * CHAR_BIT))

// one bit for every page
static size_t mem_map[MEM_BITMAP_SIZE] = { 0 };
static size_t page_map[MEM_BITMAP_SIZE] = { 0 };
static Bitset mapped_mem = Bitset(mem_map, MEM_BITMAP_SIZE);
static Bitset mapped_pages = Bitset(page_map, MEM_BITMAP_SIZE);

static uint32_t page_dir_addr;
static struct page_table* page_dir_virt[PAGE_ENTRIES];

// both of these must be page aligned for anything to work right at all
static struct page_directory_entry page_dir_phys[PAGE_ENTRIES] SECTION(".page_tables,\"aw\", @nobits#");
static struct page_table page_tables[PAGE_ENTRIES] SECTION(".page_tables,\"aw\", @nobits#");

// Function prototypes
static void mem_page_fault(struct registers* regs);
static void paging_init_dir();
static void paging_map_early_mem();
static void paging_map_hh_kernel();
static uint32_t find_next_free_virt_addr(int seq);
static uint32_t find_next_free_phys_page();
static inline void map_kernel_page_table(uint32_t pd_idx, struct page_table* table);
static inline void set_page_dir(uint32_t page_directory);
static void paging_args_cb(const char* arg);

// Kernel cmdline arguments
static bool is_mapping_output_enabled = false;
#define MAPPING_OUTPUT_FLAG "--enable-mapping-output"
KERNEL_PARAM(enable_mapping_output, MAPPING_OUTPUT_FLAG, paging_args_cb);

void paging_init(uint32_t page_count)
{
    // TODO: Get total RAM size / page count from bootloader
    machine_page_count = page_count;
    // we can set breakpoints or make a futile attempt to recover.
    register_interrupt_handler(ISR_PAGE_FAULT, mem_page_fault);
    // init our structures
    paging_init_dir();
    // identity map the first 1 MiB of RAM
    paging_map_early_mem();
    // map in our higher-half kernel
    paging_map_hh_kernel();
    // use our new set of page tables
    set_page_dir(page_dir_addr & PAGE_ALIGN);
    // flush the tlb and we're off to the races!
    Arch::pagingEnable();
}

static void mem_page_fault(struct registers* regs)
{
    PANIC(regs);
}

static inline void map_kernel_page_table(uint32_t pd_idx, struct page_table* table)
{
    page_dir_virt[pd_idx] = table;
    page_dir_phys[pd_idx] = {
        .present = 1,
        .readWrite = 1,
        .usermode = 0,
        .writeThrough = 0,
        .cacheDisable = 0,
        .accessed = 0,
        .ignoredA = 0,
        .size = 0,
        .ignoredB = 0,
        // compute the physical address of this page table
        // the virtual address is obtained with the & operator and
        // the offset is applied from the load address of the kernel
        // we must shift it over 12 bits because we only care about
        // the highest 20 bits for the page table
        .tableAddr = KADDR_TO_PHYS((uint32_t)table) >> 12
    };
}

static void paging_init_dir()
{
    // For every page in kernel memory
    for (int i = 0; i < PAGE_ENTRIES - 1; i++) {
        map_kernel_page_table(i, &page_tables[i]);
        // clear out the page tables
        for (int j = 0; j < PAGE_ENTRIES; j++) {
            page_tables[i].pages[j] = (struct page_table_entry) { /* ZERO */ };
        }
    }
    // recursively map the last page table to the page directory
    map_kernel_page_table(PAGE_ENTRIES - 1, (struct page_table*)&page_dir_phys[0]);
    for (uint32_t i = PAGE_ENTRIES * (PAGE_ENTRIES - 1); i < PAGE_ENTRIES * PAGE_ENTRIES; i++) {
        mapped_pages.Set(i);
    }
    // store the physical address of the page directory for quick access
    page_dir_addr = KADDR_TO_PHYS((uint32_t)&page_dir_phys[0]);
}

void map_kernel_page(union address vaddr, union address paddr)
{
    // Set the page directory entry (pde) and page table entry (pte)
    uint32_t pde = vaddr.page.dirIndex;
    uint32_t pte = vaddr.page.tableIndex;
    // If the page's virtual address is not aligned
    if (vaddr.page.offset != 0) {
        PANIC("Attempted to map a non-page-aligned virtual address.\n");
    }
    page_table_entry* entry = &(page_tables[pde].pages[pte]);
    // Print a debug message to serial
    if (is_mapping_output_enabled) {
        debugf("map 0x%08x to 0x%08x, pde = 0x%08x, pte = 0x%08x\n", paddr.val, vaddr.val, pde, pte);
    }
    // If the page is already mapped into memory
    if (entry->present) {
        if (entry->frame == paddr.frame.index) {
            // this page was already mapped the same way
            return;
        }

        PANIC("Attempted to map already mapped page.\n");
    }
    // Set the page information
    page_tables[pde].pages[pte] = {
        .present = 1,               // The page is present
        .readWrite = 1,             // The page has r/w permissions
        .usermode = 0,              // These are kernel pages
        .writeThrough = 0,          // Disable write through
        .cacheDisable = 0,          // The page is cached
        .accessed = 0,              // The page is unaccessed
        .dirty = 0,                 // The page is clean
        .pageAttrTable = 0,         // The page has no attribute table
        .global = 0,                // The page is local
        .unused = 0,                // Ignored
        .frame = paddr.frame.index, // The last 20 bits are the frame
    };
    // Set the associated bit in the bitmaps
    mapped_mem.Set(paddr.frame.index);
    mapped_pages.Set(vaddr.frame.index);
}

void map_kernel_range_virtual(uintptr_t begin, uintptr_t end)
{
    union address a;
    for (a = ADDR(begin); a.val < end; a.val += PAGE_SIZE) {
        map_kernel_page(a, a);
    }
}

void map_kernel_range_physical(uintptr_t begin, uintptr_t end)
{
    union address a;
    for (a = ADDR(begin); a.val < end; a.val += PAGE_SIZE) {
        union address phys {
            .val = KADDR_TO_PHYS(a.val)
        };
        map_kernel_page(a, phys);
    }
}

static void paging_map_early_mem()
{
    debugf("==== MAP EARLY MEM ====\n");
    map_kernel_range_virtual(0x0, 0x100000);
}

static void paging_map_hh_kernel()
{
    debugf("==== MAP HH KERNEL ====\n");
    map_kernel_range_physical(KERNEL_START, KERNEL_END);
}

static inline void set_page_dir(size_t page_dir)
{
    asm volatile("mov %0, %%cr3" ::"b"(page_dir));
}

/**
 * note: this can't find more than 32 sequential pages
 * @param seq the number of sequential pages to get
 */
static uint32_t find_next_free_virt_addr(int seq)
{
    return mapped_pages.FindFirstRangeClear(seq);
}

static uint32_t find_next_free_phys_page()
{
    return mapped_mem.FindFirstBitClear();
}

/**
 * map in a new page. if you request less than one page, you will get exactly one page
 */
void* get_new_page(uint32_t size)
{
    mutex_paging.Lock();
    uint32_t page_count = (size / PAGE_SIZE) + 1;
    uint32_t free_idx = find_next_free_virt_addr(page_count);
    if (free_idx == SIZE_MAX)
        return NULL;
    for (uint32_t i = free_idx; i < free_idx + page_count; i++) {
        uint32_t phys_page_idx = find_next_free_phys_page();
        if (phys_page_idx == SIZE_MAX)
            return NULL;
        union address phys = {
            .val = phys_page_idx * PAGE_SIZE,
        };
        map_kernel_page(ADDR((uint32_t)i * PAGE_SIZE), phys);
    }
    mutex_paging.Unlock();
    return (void*)(free_idx * PAGE_SIZE);
}

void free_page(void* page, uint32_t size)
{
    mutex_paging.Lock();
    uint32_t page_count = (size / PAGE_SIZE) + 1;
    uint32_t page_index = (uint32_t)page >> 12;
    for (uint32_t i = page_index; i < page_index + page_count; i++) {
        // TODO: need locking here (maybe make a paging lock)
        mapped_pages.Clear(i);
        // this is the same as the line above
        struct page_table_entry* pte = &(page_tables[i / PAGE_ENTRIES].pages[i % PAGE_ENTRIES]);
        // the frame field is actually the page frame's index
        // basically it's frame 0, 1...(2^21-1)
        mapped_mem.Clear(pte->frame);
        // zero it out to unmap it
        *pte = { /* Zero */ };
        // clear that tlb
        Arch::pageInvalidate(page);
    }
    mutex_paging.Unlock();
}

bool page_is_present(size_t addr)
{
    // Convert the address into an index and
    // check whether the page is in the bitmap
    return mapped_pages.Get(addr >> 12);
}

// TODO: maybe enforce access control here in the future
uint32_t get_phys_page_dir()
{
    return page_dir_addr;
}

static void paging_args_cb(const char* arg)
{
    if (strcmp(arg, MAPPING_OUTPUT_FLAG) == 0) {
        debugf("is_mapping_output_enabled = true");
        is_mapping_output_enabled = true;
    }
}
