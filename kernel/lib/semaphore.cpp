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

#include <lib/semaphore.hpp>
#include <lib/errno.h>
#include <mem/heap.hpp>
#include <stddef.h>

#define IS_SEMAPHORE_VALID(sem) { \
    if (sem == NULL)              \
    {                             \
        errno = EINVAL;           \
        return -1;                \
    }                             \
}

#define COMPARE_EXCHANGE(sem, curVal, failure_memorder) __atomic_compare_exchange_n( \
    &sem->count,                                                                     \
    &curVal,                                                                         \
    curVal - 1,                                                                      \
    false,                                                                           \
    __ATOMIC_RELEASE,                                                                \
    failure_memorder                                                                 \
)

px_semaphore::px_semaphore(int c, const char *name)
    : shared(false), count(c)
{
    task_sync.dbg_name = name;
}

int px_sem_init(px_sem_t *sem, bool shared, uint32_t value) {
    IS_SEMAPHORE_VALID(sem);
    sem->count = value;
    sem->shared = shared;
    px_tasks_sync_init(&sem->task_sync);
    return 0;
}

int px_sem_destroy(px_sem_t *sem) {
    IS_SEMAPHORE_VALID(sem);
    free(sem);
    return 0;
}

int px_sem_wait(px_sem_t *sem) {
    IS_SEMAPHORE_VALID(sem);
    // Used to store the current value of the semaphore for atomic comparison later.
    uint32_t curVal = sem->count;
    // Compare the semaphore's current value to the value recorded earlier.
    // If the semaphore counter is already 0 then just skip the compare and exhange.
    do
    {
        while (curVal == 0)
        {
            TASK_ONLY px_tasks_sync_block(&sem->task_sync);
            curVal = sem->count;
        }
    // Fail using atomic relaxed because it may allow us to get to the "waiting" state faster.
    } while(!COMPARE_EXCHANGE(sem, curVal, __ATOMIC_RELAXED));
    // Return success
    return 0;
}

int sem_trywait(px_sem_t *sem) {
    IS_SEMAPHORE_VALID(sem);
    // Used to store the current value of the semaphore for atomic comparison later.
    uint32_t curVal = sem->count;
    // Compare the semaphore's current value to the value recorded earlier.
    // If the semaphore counter is already 0 then just skip the compare and exhange.
    do
    {
        // If the value is 0 then go ahead and return after setting errno.
        if (curVal == 0)
        {
            // Set errno and return 0.
            errno = EAGAIN;
            return -1;
        }
    // We need to fail on an Atomic Acquire Release because it will fail less often (i.e. fewer loop iterations)
    } while(!COMPARE_EXCHANGE(sem, curVal, __ATOMIC_ACQUIRE));
    // To get here the semaphore must not have been locked.
    return 0;
}

int sem_timedwait(px_sem_t *sem, const uint32_t *usec) {
    IS_SEMAPHORE_VALID(sem);
    // TODO: Add the timer functionality here.
    (void)usec;
    // Used to store the current value of the semaphore for atomic comparison later.
    uint32_t curVal = sem->count;
    // Compare the semaphore's current value to the value recorded earlier.
    // If the semaphore counter is already 0 then just skip the compare and exhange.
    do
    {
        while (curVal == 0)
        {
            curVal = sem->count;
        }
    // Fail using atomic relaxed because it may allow us to get to the "waiting" state faster.
    } while(!COMPARE_EXCHANGE(sem, curVal, __ATOMIC_ACQUIRE));
    // Return success
    return 0;
}

int px_sem_post(px_sem_t *sem) {
    IS_SEMAPHORE_VALID(sem);
    __atomic_fetch_add(&sem->count, 1, __ATOMIC_RELEASE);
    TASK_ONLY px_tasks_sync_unblock(&sem->task_sync);
    return 0;
}

int px_sem_getval(px_sem_t *sem, uint32_t *val) {
    IS_SEMAPHORE_VALID(sem);
    __atomic_load(&sem->count, val, __ATOMIC_ACQUIRE);
    return 0;
}
