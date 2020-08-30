/**
 * @file mutex.cpp
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

int px_mutex_init(px_mutex_t *mutex) {
    // Check if the mutex is valid
    if (mutex == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    // Initialize the value to false
    mutex->locked = false;
    // Success, return 0
    return 0;
}

int px_mutex_destroy(px_mutex_t *mutex) {
    // Check if the mutex is valid
    if (mutex == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    free(mutex);
    // Success, return 0
    return 0;
}

int px_mutex_lock(px_mutex_t *mutex) {
    // Check if the mutex is valid
    if (mutex == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    // Check if the mutex is unlocked
    while (__atomic_test_and_set(&mutex->locked, __ATOMIC_RELEASE))
    {
        // Busy wait while trying to get the lock
    }
    // Success, return 0
    return 0;
}

int px_mutex_trylock(px_mutex_t *mutex) {
    // Check if the mutex is valid
    if (mutex == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    // If we cannot immediately acquire the lock then just return an error
    if (__atomic_test_and_set(&mutex->locked, __ATOMIC_RELEASE))
    {
        errno = EINVAL;
        return -1;
    }
    // Success, return 0
    return 0;
}

int px_mutex_unlock(px_mutex_t *mutex) {
    // Check if the mutex is valid
    if (mutex == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    // Clear the lock if it is locked
    __atomic_clear(&mutex->locked, __ATOMIC_RELEASE);
    // Success, return 0
    return 0;
}
