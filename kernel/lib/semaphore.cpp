/**
 * @file semaphore.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-28
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 */

// All of the GCC builtin functions used here are documented at the link provided
// below. These builtins effectly generate assembly that will atomically perform
// the function described by the function name. Normally there would be a wrapper
// around these provided by <atomic.h> but we can't use that since we don't have
// a standard library for either C or C++;
//
// Reference:
// https://gcc.gnu.org/onlinedocs/gcc-8.3.0/gcc/_005f_005fatomic-Builtins.html

#include <lib/errno.h>
#include <lib/semaphore.hpp>
#include <mem/heap.hpp>
#include <stddef.h>

// Can't make this an inline function due to compiler errors with failure_memorder
// being too strong. Likely due to the fact that it doesn't know the value at
// compile time.
#define COMPARE_EXCHANGE(curVal, failure_memorder) __atomic_compare_exchange_n( \
    &count,                                                                     \
    &curVal,                                                                    \
    curVal - 1,                                                                 \
    false,                                                                      \
    __ATOMIC_RELEASE,                                                           \
    failure_memorder)

Semaphore::Semaphore(uint32_t val, bool share, const char* name)
    : shared(share)
    , count(val)
{
    task_sync.dbg_name = name;
    tasks_sync_init(&task_sync);
}

Semaphore::~Semaphore()
{
    // Nothing to destruct
}

int Semaphore::Wait()
{
    // Used to store the current value of the semaphore for atomic comparison later.
    uint32_t curVal = count;
    // Compare the semaphore's current value to the value recorded earlier.
    // If the semaphore counter is already 0 then just skip the compare and exhange.
    do {
        while (curVal == 0) {
            TASK_ONLY tasks_sync_block(&task_sync);
            curVal = count;
        }
        // Fail using atomic relaxed because it may allow us to get to the "waiting" state faster.
    } while (!COMPARE_EXCHANGE(curVal, __ATOMIC_RELAXED));
    // Return success
    return 0;
}

int Semaphore::Trywait()
{
    // Used to store the current value of the semaphore for atomic comparison later.
    uint32_t curVal = count;
    // Compare the semaphore's current value to the value recorded earlier.
    // If the semaphore counter is already 0 then just skip the compare and exhange.
    do {
        // If the value is 0 then go ahead and return after setting errno.
        if (curVal == 0) {
            // Set errno and return 0.
            errno = EAGAIN;
            return -1;
        }
        // We need to fail on an Atomic Acquire Release because it will fail less often (i.e. fewer loop iterations)
    } while (!COMPARE_EXCHANGE(curVal, __ATOMIC_ACQUIRE));
    // To get here the semaphore must not have been locked.
    return 0;
}

int Semaphore::Timedwait(const uint32_t* usec)
{
    // TODO: Add the timer functionality here.
    (void)usec;
    // Used to store the current value of the semaphore for atomic comparison later.
    uint32_t curVal = count;
    // Compare the semaphore's current value to the value recorded earlier.
    // If the semaphore counter is already 0 then just skip the compare and exhange.
    do {
        while (curVal == 0) {
            curVal = count;
        }
        // Fail using atomic relaxed because it may allow us to get to the "waiting" state faster.
    } while (!COMPARE_EXCHANGE(curVal, __ATOMIC_ACQUIRE));
    // Return success
    return 0;
}

int Semaphore::Post()
{
    __atomic_fetch_add(&count, 1, __ATOMIC_RELEASE);
    TASK_ONLY tasks_sync_unblock(&task_sync);
    return 0;
}

int Semaphore::Count(uint32_t* val)
{
    __atomic_load(&count, val, __ATOMIC_ACQUIRE);
    return 0;
}
