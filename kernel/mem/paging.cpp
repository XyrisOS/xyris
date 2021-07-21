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

#include <sys/panic.hpp>
#include <mem/paging.hpp>
#include <lib/bitset.hpp>
#include <lib/stdio.hpp>
#include <lib/mutex.hpp>
#include <dev/serial/rs232.hpp>
#include <stddef.h>

#define KADDR_TO_PHYS(addr) ((addr) - KERNEL_BASE)

static uint32_t machine_page_count;
static Mutex mutex_paging("paging");

#define MEM_BITMAP_SIZE ((ADDRESS_SPACE_SIZE / PAGE_SIZE) / (sizeof(size_t) * CHAR_BIT))

/* one bit for every page */
static size_t mem_map[MEM_BITMAP_SIZE] = { 0 };
static size_t page_map[MEM_BITMAP_SIZE] = { 0 };
static Bitset mapped_mem = Bitset(mem_map, MEM_BITMAP_SIZE);
static Bitset mapped_pages = Bitset(page_map, MEM_BITMAP_SIZE);

static uint32_t         page_dir_addr;
static page_table_t*    page_dir_virt[PAGE_ENTRIES];

/* both of these must be page aligned for anything to work right at all */
static page_directory_entry_t page_dir_phys[PAGE_ENTRIES] __attribute__ ((section (".page_tables,\"aw\", @nobits#")));
static page_table_t           page_tables[PAGE_ENTRIES]   __attribute__ ((section (".page_tables,\"aw\", @nobits#")));

// Function prototypes
static void mem_page_fault(registers_t* regs);
static void paging_init_dir();
static void paging_map_early_mem();
static void paging_map_hh_kernel();
static uint32_t find_next_free_virt_addr(int seq);
static uint32_t find_next_free_phys_page();
static inline void map_kernel_page_table(uint32_t pd_idx, page_table_t *table);
static inline void set_page_dir(uint32_t page_directory);
static inline void paging_enable();
static inline void paging_disable();

void paging_init(uint32_t page_count) {
    machine_page_count = page_count;
    // we can set breakpoints or make a futile attempt to recover.
    register_interrupt_handler(14, mem_page_fault);
    // init our structures
    paging_init_dir();
    // identity map the first 1 MiB of RAM
    paging_map_early_mem();
    // map in our higher-half kernel
    paging_map_hh_kernel();
    // use our new set of page tables
    set_page_dir(page_dir_addr & PAGE_ALIGN);
    // flush the tlb and we're off to the races!
    paging_enable();
}

static void mem_page_fault(registers_t* regs) {
   PANIC(regs);
}

static inline void map_kernel_page_table(uint32_t pd_idx, page_table_t *table) {
    page_dir_virt[pd_idx] = table;
    page_dir_phys[pd_idx] = {
        .present = 1,
        .read_write = 1,
        .usermode = 0,
        .write_through = 0,
        .cache_disable = 0,
        .accessed = 0,
        .ignored_a = 0,
        .page_size = 0,
        .ignored_b = 0,
        // compute the physical address of this page table
        // the virtual address is obtained with the & operator and
        // the offset is applied from the load address of the kernel
        // we must shift it over 12 bits because we only care about
        // the highest 20 bits for the page table
        .table_addr = KADDR_TO_PHYS((uint32_t)table) >> 12
    };
}

static void paging_init_dir() {
    // For every page in kernel memory
    for (int i = 0; i < PAGE_ENTRIES - 1; i++) {
        map_kernel_page_table(i, &page_tables[i]);
        // clear out the page tables
        for (int j = 0; j < PAGE_ENTRIES; j++) {
            page_tables[i].pages[j] = (page_table_entry_t){ /* ZERO */ };
        }
    }
    // recursively map the last page table to the page directory
    map_kernel_page_table(PAGE_ENTRIES - 1, (page_table_t*)&page_dir_phys[0]);
    for (uint32_t i = PAGE_ENTRIES * (PAGE_ENTRIES - 1); i < PAGE_ENTRIES * PAGE_ENTRIES; i++) {
        mapped_pages.Set(i);
    }
    // store the physical address of the page directory for quick access
    page_dir_addr = KADDR_TO_PHYS((uint32_t)&page_dir_phys[0]);
}

void map_kernel_page(virtual_address_t vaddr, uint32_t paddr) {
    // Set the page directory entry (pde) and page table entry (pte)
    uint32_t pde = vaddr.page_dir_index;
    uint32_t pte = vaddr.page_table_index;
    // If the page's virtual address is not aligned
    if (vaddr.page_offset != 0) {
        PANIC("Attempted to map a non-page-aligned virtual address.\n");
    }
    page_table_entry *entry = &(page_tables[pde].pages[pte]);
    // Print a debug message to serial
    debugf("map 0x%08x to 0x%08x, pde = 0x%08x, pte = 0x%08x\n", paddr, vaddr.val, pde, pte);
    // If the page is already mapped into memory
    if (entry->present) {
        if (entry->frame == paddr >> 12) {
            // this page was already mapped the same way
            return;
        }
/*
#ifdef DEBUG
        size_t bit_idx = INDEX_FROM_BIT(vaddr.val >> 12);
#endif
        debugf(
            "pte { present = %d, read_write = %d, usermode = %d, "
            "write_through = %d,\n      cache_disable = %d, accessed = %d, "
            "dirty = %d,\n      page_att_table = %d, global = %d, frame = 0x%08x\n}\n"
            "mem_map[i-1] = 0x%08x\nmem_map[i]   = 0x%08x\nmem_map[i+1] = 0x%08x\n",
            entry->present, entry->read_write, entry->usermode, entry->write_through,
            entry->cache_disable, entry->accessed, entry->dirty, entry->page_att_table,
            entry->global, entry->frame, mapped_pages[bit_idx - 1],
            mapped_pages[bit_idx], mapped_pages[bit_idx + 1]);
*/
        PANIC("Attempted to map already mapped page.\n");
    }
    // Set the page information
    page_tables[pde].pages[pte] = {
        .present = 1,           // The page is present
        .read_write = 1,        // The page has r/w permissions
        .usermode = 0,          // These are kernel pages
        .write_through = 0,     // Disable write through
        .cache_disable = 0,     // The page is cached
        .accessed = 0,          // The page is unaccessed
        .dirty = 0,             // The page is clean
        .page_att_table = 0,    // The page has no attribute table
        .global = 0,            // The page is local
        .unused = 0,            // Ignored
        .frame = paddr >> 12    // The last 20 bits are the frame
    };
    // Set the associated bit in the bitmaps
    mapped_mem.Set(paddr >> 12);
    mapped_pages.Set(vaddr.val >> 12);
}

static void paging_map_early_mem() {
    debugf("==== MAP EARLY MEM ====\n");
    virtual_address_t a;
    for (a = VADDR(0); a.val < 0x100000; a.val += PAGE_SIZE) {
        // identity map the early memory
        map_kernel_page(a, a.val);
    }
}

static void paging_map_hh_kernel() {
    debugf("==== MAP HH KERNEL ====\n");
    virtual_address_t a;
    for (a = VADDR(KERNEL_START); a.val < KERNEL_END; a.val += PAGE_SIZE) {
        // map the higher-half kernel in
        map_kernel_page(a, KADDR_TO_PHYS(a.val));
    }
}

static inline void set_page_dir(size_t page_dir) {
    asm volatile("mov %0, %%cr3" :: "b"(page_dir));
}

static inline void paging_enable() {
    size_t cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    // 0x80000000 = 0b10000000000000000000000000000000
    // The most significant bit signifies whether to
    // enable or disable paging within control register 0.
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "b"(cr0));
}

static inline void paging_disable() {
    size_t cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    // 0x80000000 = 0b10000000000000000000000000000000
    // The most significant bit signifies whether to
    // enable or disable paging within control register 0.
    // In this case we set the opposite (~) so the result
    // is 0b01111111111111111111111111111111
    cr0 &= ~(0x80000000U);
    asm volatile("mov %0, %%cr0":: "b"(cr0));
}

/**
 * note: this can't find more than 32 sequential pages
 * @param seq the number of sequential pages to get
 */
static uint32_t find_next_free_virt_addr(int seq) {
    return mapped_pages.FindFirstRangeClear(seq);
}

static uint32_t find_next_free_phys_page() {
    return mapped_mem.FindFirstBitClear();
}

/**
 * map in a new page. if you request less than one page, you will get exactly one page
 */
void* get_new_page(uint32_t size) {
    mutex_paging.Lock();
    uint32_t page_count = (size / PAGE_SIZE) + 1;
    uint32_t free_idx = find_next_free_virt_addr(page_count);
    if (free_idx == SIZE_MAX) return NULL;
    for (uint32_t i = free_idx; i < free_idx + page_count; i++) {
        uint32_t phys_page_idx = find_next_free_phys_page();
        if (phys_page_idx == SIZE_MAX) return NULL;
        map_kernel_page(VADDR((uint32_t)i * PAGE_SIZE), phys_page_idx * PAGE_SIZE);
    }
    mutex_paging.Unlock();
    return (void *)(free_idx * PAGE_SIZE);
}

void free_page(void *page, uint32_t size) {
    mutex_paging.Lock();
    uint32_t page_count = (size / PAGE_SIZE) + 1;
    uint32_t page_index = (uint32_t)page >> 12;
    for (uint32_t i = page_index; i < page_index + page_count; i++) {
        // TODO: need locking here (maybe make a paging lock)
        mapped_pages.Clear(i);
        // how much more UN-readable can we make this?? (pls, i need to know...)
        //*(uint32_t*)((uint32_t)page_tables + i * 4) = 0;
        // this is the same as the line above
        page_table_entry_t *pte = &(page_tables[i / PAGE_ENTRIES].pages[i % PAGE_ENTRIES]);
        // the frame field is actually the page frame's index
        // basically it's frame 0, 1...(2^21-1)
        mapped_mem.Clear(pte->frame);
        // zero it out to unmap it
        *pte = { /* Zero */ };
        // clear that tlb
        invalidate_page(page);
    }
    mutex_paging.Unlock();
}

bool page_is_present(size_t addr) {
    // Convert the address into an index and
    // check whether the page is in the bitmap
    return mapped_pages.Get(addr >> 12);
}

// TODO: maybe enforce access control here in the future
uint32_t get_phys_page_dir() {
    return page_dir_addr;
}
