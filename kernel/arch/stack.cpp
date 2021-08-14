/**
 * @file stack.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Stack management functionality
 * @version 0.1
 * @date 2021-02-21
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <arch/stack.hpp>
#include <sys/panic.hpp>
#include <stdint.h>

/**
 * @brief This function is the global handler for all
 * stack protection. GCC will automatically write the
 * canary code and use this function as the handler
 * for when a smashed stack is detected.
 *
 */
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;
extern "C" void __stack_chk_fail(void)
{
    PANIC("Smashed stack detected.");
}
