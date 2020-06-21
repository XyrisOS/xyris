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

#define SET_PAGE_MAPPED(addr) mapped_pages[INDEX_FROM_BIT((addr) & PAGE_ALIGN / PAGE_SIZE)] \
    |= 1UL << OFFSET_FROM_BIT((addr) / PAGE_SIZE)
#define SET_PAGE_UNMAPPED(addr) mapped_pages[INDEX_FROM_BIT((addr) & PAGE_ALIGN / PAGE_SIZE)] \
    &= ~(1UL << OFFSET_FROM_BIT((addr) / PAGE_SIZE))

/* one bit for every page */
static uint32_t mapped_pages[ADDRESS_SPACE_SIZE / PAGE_SIZE / (sizeof(uint32_t) * 8)] = { 0 };

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

void px_paging_init_dir() {
    for (int i = 0; i < PAGE_ENTRIES - 1; i++) {
        px_map_kern_page_table(i, &page_tables[i]);
    }
    // recursivly map the last page table to the page directory
    px_map_kern_page_table(PAGE_ENTRIES - 1, (px_page_table_t*)&page_dir_phys[0]);
    // store the physical address of the page directory for quick access
    page_dir_addr = KADDR_TO_PHYS((uint32_t)&page_dir_phys[0]);
}

void px_paging_map_early_mem() {
   for (uint32_t i = 0; i < 0x100000; i += PAGE_SIZE) {
        page_tables[0].pages[i / PAGE_SIZE] = {
            .present = 1,
            .read_write = 1,
            .usermode = 0,
            .frame = KADDR_TO_PHYS(i) >> 12
        };
        SET_PAGE_MAPPED(i);
    }
}

void px_paging_map_hh_kernel() {
    for (uint32_t addr = KERNEL_START; addr < KERNEL_END; addr += PAGE_SIZE) {
        uint32_t pde = addr >> 22;
        uint32_t pte = (addr >> 12) & 0b1111111111;
        page_tables[pde].pages[pte] = {
            .present = 1,
            .read_write = 1,
            .usermode = 0,
            .frame = KADDR_TO_PHYS(addr) >> 12
        };
        SET_PAGE_MAPPED(addr);
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
    px_paging_map_early_mem();
    px_paging_map_hh_kernel();
    // use our new set of page tables
    px_set_pd(page_dir_addr & PAGE_ALIGN);
    // flush the tlb and we're off to the races!
    px_paging_enable();
}

