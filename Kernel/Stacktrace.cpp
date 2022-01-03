/**
 * @file trace.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Kernel stack tracing
 * @version 0.3
 * @date 2020-08-09
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */

#include <Arch/Arch.hpp>
#include <Devices/Graphics/console.hpp>
#include <Devices/Serial/rs232.hpp>
#include <Library/stdio.hpp>
#include <Memory/paging.hpp>
#include <Stacktrace.hpp>

#define STACK_TRACE_BUF_SZ 32

namespace Stack {

void printTrace(size_t max)
{
    // Define our stack
    struct Arch::stackframe* stk;
    asm volatile("movl %%ebp, %0" : "=r"(stk)::);
    Console::printf("\033[31mStack trace:\033[0m\n");
    RS232::printf("\033[31mStack trace:\033[0m\n");

    char buf[STACK_TRACE_BUF_SZ];
    for (size_t frame = 0; stk != NULL && frame < max; ++frame) {
        // Unwind to previous stack frame
        ksprintf(buf, "0x%08X\n", stk->eip);
        Console::printf("%s", buf);
        RS232::printf("%s", buf);
        // Check for a NULL stack frame to indicate the end
        if ((uintptr_t)stk->ebp == 0x00000000) {
            break;
        }
        stk = stk->ebp;
    }
}

} // !namespace Stack
