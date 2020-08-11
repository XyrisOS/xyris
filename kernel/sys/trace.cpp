/**
 * @file trace.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-08-09
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */

#include <sys/trace.hpp>
#include <lib/stdio.hpp>
#include <arch/arch.hpp>

void px_stack_trace(size_t max) {
    // Define our stack
    struct stackframe *stk;
    asm volatile("movl %%ebp, %0" : "=r"(stk) ::);
    px_kprintf("\nStack trace:\n");
    for (size_t frame = 0; stk != NULL && frame < max; ++frame) {
        // Unwind to previous stack frame
        px_kprintf("0x%08X\n", stk->eip);
        stk = stk->ebp;
    }
}