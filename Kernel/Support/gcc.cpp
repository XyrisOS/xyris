/**
 * @file cxx.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief local static variable guards
 * @version 0.1
 * @date 2021-07-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 * References:
 *     https://wiki.osdev.org/C++#Local_Static_Variables_.28GCC_Only.29
 *
 */
#include <Arch/Arch.hpp>
#include <Panic.hpp>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief This function is the global handler for all stack protection. GCC will automatically
 * write the canary code and use this function as the handler for when a smashed stack is detected.
 *
 */
#if UINT32_MAX == UINTPTR_MAX
uintptr_t __stack_chk_guard = 0xDEADC0DE;
#else
uintptr_t __stack_chk_guard = 0xBADBADBADBADBAD1;
#endif

extern "C"
{

// Function prototypes (to make compiler happy)
void __stack_chk_fail(void);
void __cxa_pure_virtual();

void __stack_chk_fail(void)
{
    panic("Smashed stack detected.");
}

void __cxa_pure_virtual()
{
    panic("Pure virtual called!");
}

}

/**
 * These static variable guard functions are only used by GCC (according to the Wiki). If Xyris ever
 * switches to or supports Clang, we want to be prepared.
 */
#ifdef __GNUG__
namespace __cxxabiv1 {

// The ABI requires a 64-bit type.
__extension__ typedef int64_t __guard [[gnu::mode(__DI__)]];

extern "C"
{

// Function prototypes
int __cxa_guard_acquire(__guard* g);
void __cxa_guard_release(__guard* g);
void __cxa_guard_abort(__guard* g);

int __cxa_guard_acquire(__guard* g)
{
    return __atomic_test_and_set(g, __ATOMIC_RELEASE);
}

void __cxa_guard_release(__guard* g)
{
    __atomic_clear(g, __ATOMIC_RELEASE);
}

void __cxa_guard_abort(__guard* g)
{
    __atomic_clear(g, __ATOMIC_RELEASE);
}

}

} // !namespace __cxxabiv1
#endif
