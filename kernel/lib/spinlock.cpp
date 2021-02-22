/**
 * @file spinlock.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Spinlocks are implemented as mutexes which spin
 * in the current task instead of blocking and informing the
 * scheduler.
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

#include <lib/spinlock.hpp>
#include <lib/errno.h>
#include <mem/heap.hpp>
#include <stddef.h>

#define ACQUIRE_SPINLOCK_LOCK(spinlock) __atomic_test_and_set(&spinlock->locked, __ATOMIC_RELEASE)

#define IS_SPINLOCK_VALID(spinlock) { \
    if (spinlock == NULL)          \
    {                           \
        errno = EINVAL;         \
        return -1;              \
    }                           \
}

px_spinlock::px_spinlock(const char *name)
    : locked(false)
{
    task_sync.dbg_name = name;
};

int px_spinlock_init(px_spinlock_t *spinlock) {
    IS_SPINLOCK_VALID(spinlock);
    // Initialize the value to false
    spinlock->locked = false;
    px_tasks_sync_init(&spinlock->task_sync);
    // Success, return 0
    return 0;
}

int px_spinlock_destroy(px_spinlock_t *spinlock) {
    IS_SPINLOCK_VALID(spinlock);
    free(spinlock);
    // Success, return 0
    return 0;
}

int px_spinlock_lock(px_spinlock_t *spinlock) {
    IS_SPINLOCK_VALID(spinlock);
    // Check if the spinlock is unlocked
    while (ACQUIRE_SPINLOCK_LOCK(spinlock))
    {
        // Busy wait while trying to get the lock
        asm("pause");
    }
    // Success, return 0
    return 0;
}

int px_spinlock_trylock(px_spinlock_t *spinlock) {
    IS_SPINLOCK_VALID(spinlock);
    // If we cannot immediately acquire the lock then just return an error
    if (ACQUIRE_SPINLOCK_LOCK(spinlock))
    {
        errno = EINVAL;
        return -1;
    }
    // Success, return 0
    return 0;
}

int px_spinlock_unlock(px_spinlock_t *spinlock) {
    IS_SPINLOCK_VALID(spinlock);
    // Clear the lock
    __atomic_clear(&spinlock->locked, __ATOMIC_RELEASE);
    TASK_ONLY px_tasks_sync_unblock(&spinlock->task_sync);
    // Success, return 0
    return 0;
}
