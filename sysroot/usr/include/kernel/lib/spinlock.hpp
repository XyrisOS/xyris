/**
 * @file spinlock.hpp
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
#pragma once

#include <stdint.h>
#include <sys/tasks.hpp>

typedef struct px_spinlock {
    bool locked;
    px_tasks_sync_t task_sync;
    px_spinlock(const char *name = nullptr);
} px_spinlock_t;

/**
 * @brief Initializes a spinlock mutex for further use.
 *
 * @param mutex Reference spinlock
 * @return int Returns 0 on success and -1 on error.
 */
int px_spinlock_init(px_spinlock_t *spinlock);
/**
 * @brief Destroys a spinlock mutex and removes it from memory.
 *
 * @param mutex Reference spinlock
 * @return int Returns 0 on success and -1 on error.
 */
int px_spinlock_destroy(px_spinlock_t *spinlock);
/**
 * @brief Locks a provided spinlock. This call will block
 * if the spinlock is already locked.
 * 
 * @param mutex Reference spinlock
 * @return int Returns 0 on success and -1 on error.
 */
int px_spinlock_lock(px_spinlock_t *spinlock);
/**
 * @brief Attempts to lock a spinlock. If the spinlock is
 * currently locked then the function will return and
 * set errno accordingly.
 * 
 * @param mutex Reference spinlock
 * @return int Returns 0 on success and -1 on error.
 */
int px_spinlock_trylock(px_spinlock_t *spinlock);
/**
 * @brief Unlocks a spinlock for others to use.
 *
 * @param mutex Reference spinlock
 * @return int Returns 0 on success and -1 on error.
 */
int px_spinlock_unlock(px_spinlock_t *spinlock);
