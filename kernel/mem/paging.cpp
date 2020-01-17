/**
 * @file paging.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html
 * @version 0.1
 * @date 2019-11-22
 *
 * @copyright Copyright Keeton Feavel et al (c) 2019
 *
 */

#include <mem/paging.hpp>

static void px_frame_set(uint32_t frame_addr);
static void px_frame_clear(uint32_t frame_addr);
static uint32_t px_frame_test(uint32_t frame_addr);
static uint32_t px_frame_get_first();
void px_paging_init();
void px_frame_alloc(px_page_table_entry_t *page, int is_kernel, int is_writeable);
void px_frame_free(px_page_table_entry_t *page);
void px_page_switch_dir(page_directory_t *dir);
void px_mem_page_fault(registers_t* regs);
px_page_table_entry_t *px_mem_get_page(uint32_t address, int make, page_directory_t *dir);

// Kernel page directories
page_directory_t* kernel_directory = 0;
page_directory_t* current_directory = 0;
// A bitset of frames - used or free.
uint32_t *frames;
uint32_t num_frames;
//
extern size_t base_curr;

void* px_mem_physaddr(px_virtual_address virt_addr) {
  // We're given a 32 bit virtual address, so we need to translate that into a virtual address
  // 31   (10 bits)   22 | 21 (10 bits) 12 | 11 (12 bits) 0
  // [ Directory Entry ] | [ Table Entry ] | [   Offset   ]

  // Here you need to check whether the page directory entry is present.
  unsigned long * page_dir = (unsigned long *)0xFFFFF000;

  // Here you need to check whether the page table entry is present.
  unsigned long * page_tbl = ((unsigned long *)0xFFC00000) + (0x400 * virt_addr.page_dir_index);

  // Cut off all of the extra flags (12 bits) we get from the page table entry and align.
  size_t kb_aligned = (page_tbl[virt_addr.page_table_index] & 0xFFFFF000);
  // Return a pointer to the address as a void type because #YOLO.
  return (void *)(kb_aligned + virt_addr.page_offset);
}

// Static function to set a bit in the frames bitset
static void px_frame_set(uint32_t frame_addr) {
  // Stubbed
}

// Static function to clear a bit in the frames bitset
static void px_frame_clear(uint32_t frame_addr) {
  // Stubbed
}

// Static function to test if a bit is set.
static uint32_t px_frame_test(uint32_t frame_addr) {
  // Stubbed
}

// Static function to find the first free frame.
static uint32_t px_frame_get_first() {
    // Stubbed
}

void px_paging_init() {
    // The size of physical memory. For the moment we assume it is 16MB big.
    size_t mem_end_page = 0x1000000;
    num_frames = mem_end_page / 0x1000;
    frames = (size_t*)px_kmalloc(INDEX_FROM_BIT(num_frames));
    memset(frames, 0, INDEX_FROM_BIT(num_frames));
    // Create kernel page directory
    kernel_directory = (page_directory_t*)px_kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;
    // We need to identity map (phys addr = virt addr) from
    // 0x0 to the end of used memory, so we can access this
    // transparently, as if paging wasn't enabled.
    // NOTE that we use a while loop here deliberately.
    // inside the loop body we actually change placement_address
    // by calling kmalloc(). A while loop causes this to be
    // computed on-the-fly rather than once at the start.
    // -- James Molloy
    for (int i = 0; i < base_curr; i += 0x1000) {
        // Kernel code is readable but not writeable from userspace.
        px_frame_alloc(px_mem_get_page(i, 1, kernel_directory), 0, 0);
    }
    // Register our page fault handler before we enable paging so that
    // we can set breakpoints or make a futile attempt to recover.
    px_register_interrupt_handler(14, px_mem_page_fault);
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

// Function to allocate a frame.
void px_frame_alloc(px_page_table_entry_t *page, int is_kernel, int is_writeable) {
   // Stubbed
}

// Function to deallocate a frame.
void px_frame_free(px_page_table_entry_t *page) {
   // Stubbed
}

void px_page_switch_dir(page_directory_t *dir) {
   // Stubbed
}

px_page_table_entry_t *px_mem_get_page(uint32_t address, int make, page_directory_t *dir) {
    // Stubbed
}

void px_mem_page_fault(registers_t* regs) {
   PANIC(*regs);
}