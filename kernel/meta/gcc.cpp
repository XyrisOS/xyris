/**
 * @file cxx.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief local static variable guards
 * @version 0.1
 * @date 2021-07-24
 * 
 * @copyright Copyright the Panix Contributors (c) 2021
 * 
 * References:
 *     https://wiki.osdev.org/C++#Local_Static_Variables_.28GCC_Only.29
 *
 */
#include <stddef.h>
#include <stdint.h>
#include <sys/panic.hpp>

// Function prototypes
extern "C" void __cxa_pure_virtual();

// These static variable guard functions are only used by
// GCC (according to the Wiki). If Panix ever switches to
// or supports Clang, we want to be prepared.
#ifdef __GNUG__
namespace __cxxabiv1 {

// The ABI requires a 64-bit type.
__extension__ typedef int64_t __guard __attribute__((mode(__DI__)));

// Function prototypes
extern "C" int __cxa_guard_acquire(__guard* g);
extern "C" void __cxa_guard_release(__guard* g);
extern "C" void __cxa_guard_abort(__guard* g);

extern "C" int __cxa_guard_acquire(__guard* g)
{
    return __atomic_test_and_set(g, __ATOMIC_RELEASE);
}

extern "C" void __cxa_guard_release(__guard* g)
{
    __atomic_clear(g, __ATOMIC_RELEASE);
}

extern "C" void __cxa_guard_abort(__guard* g)
{
    __atomic_clear(g, __ATOMIC_RELEASE);
}

} // !__cxxabiv1
#endif

extern "C" void __cxa_pure_virtual()
{
    PANIC("Pure virtual called!");
}
