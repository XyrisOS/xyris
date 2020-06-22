/**
 * @file paging.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2019-11-22
 *
 * @copyright Copyright Keeton Feavel et al (c) 2019
 *
 */

#include <mem/paging.hpp>

#define KADDR_TO_PHYS(addr) ((addr) - KERNEL_BASE)

#define SET_BIT(bmp, addr) (bmp)[INDEX_FROM_BIT((addr) & PAGE_ALIGN / PAGE_SIZE)] \
    |= 1UL << OFFSET_FROM_BIT((addr) / PAGE_SIZE)
#define UNSET_BIT(bpm, addr) (bmp)[INDEX_FROM_BIT((addr) & PAGE_ALIGN / PAGE_SIZE)] \
    &= ~(1UL << OFFSET_FROM_BIT((addr) / PAGE_SIZE))

#define BITMAP_SIZE ADDRESS_SPACE_SIZE / PAGE_SIZE / (sizeof(uint32_t) * 8)

/* one bit for every page */
static uint32_t mapped_mem[BITMAP_SIZE] = { 0 };
static uint32_t mapped_pages[BITMAP_SIZE] = { 0 };

static uint32_t                  page_dir_addr;
static px_page_table_t *         page_dir_virt[PAGE_ENTRIES];

/* both of these must be page aligned for anything to work right at all */
static px_page_directory_entry_t page_dir_phys[PAGE_ENTRIES] __attribute__ ((section (".page_tables")));
static px_page_table_t           page_tables[PAGE_ENTRIES]   __attribute__ ((section (".page_tables")));

void px_paging_init();

static void px_mem_page_fault(registers_t* regs) {
   PANIC(regs);
}

static inline void px_map_kern_page_table(uint32_t pd_idx, px_page_table_t *table) {
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
    for (int i = 0; i < PAGE_ENTRIES - 1; i++) {
        px_map_kern_page_table(i, &page_tables[i]);
    }
    // recursivly map the last page table to the page directory
    px_map_kern_page_table(PAGE_ENTRIES - 1, (px_page_table_t*)&page_dir_phys[0]);
    // store the physical address of the page directory for quick access
    page_dir_addr = KADDR_TO_PHYS((uint32_t)&page_dir_phys[0]);
}

static void px_map_kern_page(px_virtual_address_t vaddr, uint32_t paddr) {
    uint32_t pde = vaddr.page_dir_index;
    uint32_t pte = vaddr.page_table_index;
    if (vaddr.page_offset) {
        PANIC("attempted to map a non-page-aligned virtual address");
    }
    if (page_tables[pde].pages[pte].present) {
        PANIC("attempted to map already mapped page");
    }
    page_tables[pde].pages[pte] = {
        .present = 1,
        .read_write = 1,
        .usermode = 0,
        .frame = paddr >> 12
    };
    SET_BIT(mapped_mem, paddr);
    SET_BIT(mapped_pages, vaddr.intval);
}

static void px_paging_map_early_mem() {
    for (px_virtual_address_t i = { .intval = 0 }; i.intval < 0x100000; i.intval += PAGE_SIZE) {
        // identity map the early memory
        px_map_kern_page(i, i.intval);
    }
}

static void px_paging_map_hh_kernel() {
    for (px_virtual_address_t addr = { .intval = KERNEL_START }; addr.intval < KERNEL_END; addr.intval += PAGE_SIZE) {
        // map the higher-half kernel in
        px_map_kern_page(addr, KADDR_TO_PHYS(addr.intval));
    }
}

static inline void px_set_pd(uint32_t page_directory) {
    asm volatile("mov %0, %%cr3" :: "b"(page_directory));
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

void px_paging_init() {
    // we can set breakpoints or make a futile attempt to recover.
    px_register_interrupt_handler(14, px_mem_page_fault);
    // init our structures
    px_paging_init_dir();
    // identity map the first 1 MiB of RAM
    px_paging_map_early_mem();
    // map in our higher-half kernel
    px_paging_map_hh_kernel();
    // use our new set of page tables
    px_set_pd(page_dir_addr & PAGE_ALIGN);
    // flush the tlb and we're off to the races!
    px_paging_enable();
}

