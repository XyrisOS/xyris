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

void px_paging_init();
void px_mem_page_fault(registers_t* regs);

void px_mem_page_fault(registers_t* regs) {
   PANIC(regs);
}

void px_paging_init() {
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