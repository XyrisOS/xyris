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

#include "Logger.hpp"
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
    struct Arch::stackframe* frame;
    asm volatile(
        "movl %%ebp, %0"
        : "=r"(frame));
    Console::printf("\033[31mStack trace:\033[0m\n");
    Logger::Print("\033[31mStack trace:\033[0m\n");

    for (size_t i = 0; frame != NULL && i < max; ++i) {
        Console::printf("0x%08zX\n", frame->eip);
        Logger::Print("0x%08zX", frame->eip);
        if ((uintptr_t)frame->ebp == 0x00000000) {
            break;
        }
        frame = frame->ebp;
    }
}

} // !namespace Stack
