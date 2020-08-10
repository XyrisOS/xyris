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

void px_stack_trace(unsigned int max) {
    // Define our stack
    struct stackframe *stk;
    asm volatile("movl %%ebp,%0" : "=r"(stk) ::);
    px_kprintf("\nStack trace:\n");
    for(unsigned int frame = 0; stk && frame < max; ++frame) {
        // Unwind to previous stack frame
        px_kprintf("0x%08X\n", stk->eip);
        stk = stk->ebp;
    }
}