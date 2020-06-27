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

#include <mem/paging.hpp>

#define KADDR_TO_PHYS(addr) ((addr) - KERNEL_BASE)

#define SET_BIT_IN_MAP(bmp, addr) (bmp)[INDEX_FROM_BIT(((addr) & PAGE_ALIGN) / PAGE_SIZE)] \
    |= 1UL << OFFSET_FROM_BIT((addr) / PAGE_SIZE)

#define UNSET_BIT_IN_MAP(bmp, addr) (bmp)[INDEX_FROM_BIT(((addr) & PAGE_ALIGN) / PAGE_SIZE)] \
    &= ~(1UL << OFFSET_FROM_BIT((addr) / PAGE_SIZE))

#define BITMAP_SIZE (ADDRESS_SPACE_SIZE / PAGE_SIZE / (sizeof(uint32_t) * 8))

static uint32_t machine_page_count;

/* one bit for every page */
static uint32_t mapped_mem[BITMAP_SIZE] = { 0 };
static uint32_t mapped_pages[BITMAP_SIZE] = { 0 };

static uint32_t                  page_dir_addr;
static px_page_table_t *         page_dir_virt[PAGE_ENTRIES];

/* both of these must be page aligned for anything to work right at all */
static px_page_directory_entry_t page_dir_phys[PAGE_ENTRIES] __attribute__ ((section (".page_tables")));
static px_page_table_t           page_tables[PAGE_ENTRIES]   __attribute__ ((section (".page_tables")));

// Function prototypes
static void px_mem_page_fault(registers_t* regs);
static void px_paging_init_dir();
static void px_map_kernel_page(px_virtual_address_t vaddr, uint32_t paddr);
static void px_paging_map_early_mem();
static void px_paging_map_hh_kernel();
static int find_next_free_virt_addr(int seq);
static int find_next_free_phys_page();
static inline void px_map_kernel_page_table(uint32_t pd_idx, px_page_table_t *table);
static inline void px_set_page_dir(uint32_t page_directory);
static inline void px_paging_enable();
static inline void px_paging_disable();

void px_paging_init(uint32_t page_count) {
    machine_page_count = page_count;
    // we can set breakpoints or make a futile attempt to recover.
    px_register_interrupt_handler(14, px_mem_page_fault);
    // init our structures
    px_paging_init_dir();
    // identity map the first 1 MiB of RAM
    px_paging_map_early_mem();
    // map in our higher-half kernel
    px_paging_map_hh_kernel();
    // use our new set of page tables
    px_set_page_dir(page_dir_addr & PAGE_ALIGN);
    // flush the tlb and we're off to the races!
    px_paging_enable();
}

static void px_mem_page_fault(registers_t* regs) {
   PANIC(regs);
}

static inline void px_map_kernel_page_table(uint32_t pd_idx, px_page_table_t *table) {
    page_dir_virt[pd_idx] = table;
    page_dir_phys[pd_idx] = {
        .present = 1,
        .read_write = 1,
        .usermode = 0,
        // compute the physical address of this page table
        // the virtual address is obtained with the & operator and
        // the offset is applied from the load address of the kernel
        // we must shift it over 12 bits because we only care about
        // the highest 20 bits for the page table
        .table_addr = KADDR_TO_PHYS((uint32_t)table) >> 12
    };
}

static void px_paging_init_dir() {
    // For every page in kernel memory
    for (int i = 0; i < PAGE_ENTRIES - 1; i++) {
        px_map_kernel_page_table(i, &page_tables[i]);
    }
    // recursivly map the last page table to the page directory
    px_map_kernel_page_table(PAGE_ENTRIES - 1, (px_page_table_t*)&page_dir_phys[0]);
    // store the physical address of the page directory for quick access
    page_dir_addr = KADDR_TO_PHYS((uint32_t)&page_dir_phys[0]);
}

static void px_map_kernel_page(px_virtual_address_t vaddr, uint32_t paddr) {
    // Set the page directory entry (pde) and page table entry (pte)
    uint32_t pde = vaddr.page_dir_index;
    uint32_t pte = vaddr.page_table_index;
    // If the page's virtual address is not aligned
    if (vaddr.page_offset) {
        PANIC("Attempted to map a non-page-aligned virtual address.");
    }
    // If the page is already mapped into memory
    if (page_tables[pde].pages[pte].present) {
        PANIC("Attempted to map already mapped page.");
    }
    // Set the page information
    page_tables[pde].pages[pte] = {
        .present = 1,           // The page is present
        .read_write = 1,        // The page has r/w permissions
        .usermode = 0,          // These are kernel pages
        .frame = paddr >> 12    // The last 12 bits are the frame
    };
    // Set the associated bit in the bitmaps
    SET_BIT_IN_MAP(mapped_mem, paddr);
    SET_BIT_IN_MAP(mapped_pages, vaddr.val);
}

static void px_paging_map_early_mem() {
    px_virtual_address_t a;
    for (a = VADDR(0); a.val < 0x100000; a.val += PAGE_SIZE) {
        // identity map the early memory
        px_map_kernel_page(a, a.val);
    }
}

static void px_paging_map_hh_kernel() {
    px_virtual_address_t addr;
    for (addr = VADDR(KERNEL_START); addr.val < KERNEL_END; addr.val += PAGE_SIZE) {
        // map the higher-half kernel in
        px_map_kernel_page(addr, KADDR_TO_PHYS(addr.val));
    }
}

static inline void px_set_page_dir(uint32_t page_dir) {
    asm volatile("mov %0, %%cr3" :: "b"(page_dir));
}

static inline void px_paging_enable() {
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "b"(cr0));
}

static inline void px_paging_disable() {
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    cr0 &= ~(0x80000000U);
    asm volatile("mov %0, %%cr0":: "b"(cr0));
}

/**
 * note: this can't find more than 32 sequential pages
 * @param seq the number of sequential pages to get
 */
static int find_next_free_virt_addr(int seq) {
    uint32_t check;
    uint32_t mask = (1 << seq) - 1;
    for (int i = 0; i < (ADDRESS_SPACE_SIZE / PAGE_SIZE); i++) {
        check = mapped_mem[i / 32] >> (i % 32);
        check |= mapped_mem[(i / 32) + 1] << (32 - (i % 32));
        if (!(check & mask)) return i;
    }
    return -1;
}

static int find_next_free_phys_page() {
    for (int i = 0; i < (ADDRESS_SPACE_SIZE / PAGE_SIZE); i++) {
        if (!((mapped_pages[INDEX_FROM_BIT(i)] >> OFFSET_FROM_BIT(i)) & 1)) return i;
    }
    return -1;
}

/**
 * map in a new page. if you request less than one page, you will get exactly one page
 */
void* px_get_new_page(uint32_t size) {
    int page_count = (size / PAGE_SIZE) + 1;
    int free_idx = find_next_free_virt_addr(page_count);
    if (free_idx == -1) return NULL;
    for (int i = free_idx; i < free_idx + page_count; i++) {
        int phys_page_idx = find_next_free_phys_page();
        if (phys_page_idx == -1) return NULL;
        px_map_kernel_page(VADDR((uint32_t)i * PAGE_SIZE), phys_page_idx * PAGE_SIZE);
    }
    return (void *)(free_idx * PAGE_SIZE);
}

void px_free_page(void *page, uint32_t size) {
    int page_count = (size / PAGE_SIZE) + 1;
    for (uint32_t i = (uint32_t)page >> 12; i < ((uint32_t)page >> 12) + page_count; i++) {
        UNSET_BIT_IN_MAP(mapped_pages, i);
        // how much more UN-readable can we make this?? (pls, i need to know...)
        //*(uint32_t*)((uint32_t)page_tables + i * 4) = 0;
        // this is the same as the line above
        px_page_table_entry_t *pte = &(page_tables[i / PAGE_ENTRIES].pages[i % PAGE_ENTRIES]);
        // the frame field is actually the page frame's index
        // basically it's frame 0, 1...(2^21-1)
        UNSET_BIT_IN_MAP(mapped_mem, pte->frame);
        // zero it out to unmap it
        *pte = (px_page_table_entry_t){ 0 };
        // clear that tlb
        px_invalidate_page(page);
    }
}

