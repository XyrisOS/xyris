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
void px_mem_page_fault(registers_t regs);
px_page_table_entry_t *px_mem_get_page(uint32_t address, int make, page_directory_t *dir);

// Kernel page directories
page_directory_t* kernel_directory = 0;
page_directory_t* current_directory = 0;
// A bitset of frames - used or free.
uint32_t *frames;
uint32_t nframes;

// Static function to set a bit in the frames bitset
static void px_frame_set(uint32_t frame_addr) {
}

// Static function to clear a bit in the frames bitset
static void px_frame_clear(uint32_t frame_addr) {
}

// Static function to test if a bit is set.
static uint32_t px_frame_test(uint32_t frame_addr) {
}

// Static function to find the first free frame.
static uint32_t px_frame_get_first() {
    
}

void px_paging_init() {
   
}

// Function to allocate a frame.
void px_frame_alloc(px_page_table_entry_t *page, int is_kernel, int is_writeable) {
   
}

// Function to deallocate a frame.
void px_frame_free(px_page_table_entry_t *page) {
   
}

void px_page_switch_dir(page_directory_t *dir) {
   
}

px_page_table_entry_t *px_mem_get_page(uint32_t address, int make, page_directory_t *dir) {
    
}

void px_mem_page_fault(registers_t regs) {
   PANIC(regs);
}