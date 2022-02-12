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
 *     https://opensource.apple.com/source/libcppabi/libcppabi-14/src/cxa_guard.cxx
 *
 */
#include <Locking/Mutex.hpp>
#include <Arch/Arch.hpp>
#include <Panic.hpp>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief This function is the global handler for all stack protection. GCC will automatically
 * write the canary code and use this function as the handler for when a smashed stack is detected.
 *
 */
#if UINTPTR_MAX == UINT32_MAX
uintptr_t __stack_chk_guard = 0xDEADC0DE;
#elif UINTPTR_MAX == UINT64_MAX
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
 * Special thanks to the LLVM maintainers and Apple for their fantastic comments explaining what these
 * functions are supposed to do, as well as providing implementation inspiration. See the link at the
 * top of this source file for details.
 */
namespace __cxxabiv1 {

/**
 * All static object initializations are protected by the single, global, recursive mutex.
 * The recursive mutex is apparently required to enable nested objects initialization
 *
 * FIXME: This needs to be a recursive lock when those are added
 */
Mutex cxaGuard("cxaguard");

struct cxaguard {
    uint64_t initialized : 8;
    uint64_t inuse : 8;
    uint64_t guard : 48;
};

// The ABI requires a 64-bit type.
__extension__ typedef struct cxaguard __guard;

static void cxaGuardSetInitializeHasRun(__guard* g)
{
    g->initialized = 1;
}

static bool cxaGuardHasInitializerRun(__guard* g)
{
    return g->initialized;
}

static void cxaGuardSetInUse(__guard* g)
{
    g->inuse = 1;
}

static void cxaGuardSetNotInUse(__guard* g)
{
    g->inuse = 0;
}

static bool cxaGuardIsInUse(__guard* g)
{
    return g->inuse;
}

extern "C"
{

// Function prototypes
int __cxa_guard_acquire(__guard* g);
void __cxa_guard_release(__guard* g);
void __cxa_guard_abort(__guard* g);

int __cxa_guard_acquire(__guard* g)
{
    if (cxaGuardHasInitializerRun(g)) {
        return 0;
    }

    cxaGuard.lock();
    // Ensure another thread didn't beat us to the chase
    if (cxaGuardHasInitializerRun(g)) {
        cxaGuard.unlock();
        return 0;
    }

    if (cxaGuardIsInUse(g)) {
        panicf("[%s] __guard in bad state", __func__);
    }

    cxaGuardSetInUse(g);
    return 1;
}

void __cxa_guard_release(__guard* g)
{
    cxaGuardSetInitializeHasRun(g);
    cxaGuard.unlock();
}

void __cxa_guard_abort(__guard* g)
{
    cxaGuard.unlock();
    cxaGuardSetNotInUse(g);
}

}

} // !namespace __cxxabiv1
