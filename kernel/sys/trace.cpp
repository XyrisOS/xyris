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

#include <arch/Arch.hpp>
#include <dev/graphics/console.hpp>
#include <dev/serial/rs232.hpp>
#include <lib/stdio.hpp>
#include <mem/paging.hpp>
#include <sys/trace.hpp>

void stack_trace(size_t max)
{
    // Define our stack
    struct Arch::stackframe* stk;
    asm volatile("movl %%ebp, %0"
                 : "=r"(stk)::);
    // TODO: Get width of "screen" (replace 80 with width) (#275)
    Console::printf("\033[0;%iH \033[31mStack trace:\033[0m\n", (80 - 16));
    RS232::printf("%s", "\n\033[31mStack trace:\033[0m\n");
    char buf[32];
    for (size_t frame = 0; stk != NULL && frame < max; ++frame) {
        // Unwind to previous stack frame
        ksprintf(buf, "0x%08X\n", stk->eip);
        Console::printf("\033[%lu;%iH  %s", (frame + 1), (80 - 16), buf);
        RS232::printf("%s", buf);
        // Check whether the address is in memory or not
        if (!Memory::isPresent((size_t)stk->ebp))
            break;
        stk = stk->ebp;
    }
}
