/**
 * @file Mutex.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-30
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

#include <lib/mutex.hpp>
#include <lib/errno.h>
#include <mem/heap.hpp>
#include <stddef.h>

Mutex::Mutex(const char* name)
    : locked(false)
{
    task_sync.dbg_name = name;
    tasks_sync_init(&task_sync);
};

Mutex::~Mutex()
{
    // Nothing to destruct yet
}

int Mutex::Lock()
{
    // Check if the Mutex is unlocked
    while (__atomic_test_and_set(&locked, __ATOMIC_RELEASE)) {
        // Block the current kernel task
        TASK_ONLY tasks_sync_block(&task_sync);
    }
    // Success, return 0
    return 0;
}

int Mutex::Trylock()
{
    // If we cannot immediately acquire the lock then just return an error
    if (__atomic_test_and_set(&locked, __ATOMIC_RELEASE)) {
        errno = EINVAL;
        return -1;
    }
    // Success, return 0
    return 0;
}

int Mutex::Unlock()
{
    // Clear the lock
    __atomic_clear(&locked, __ATOMIC_RELEASE);
    TASK_ONLY tasks_sync_unblock(&task_sync);
    // Success, return 0
    return 0;
}
