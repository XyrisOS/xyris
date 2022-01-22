/**
 * @file Mutex.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-30
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
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

#include <Locking/Mutex.hpp>

Mutex::Mutex(const char* name)
    : m_isLocked(false)
{
    m_taskSync.dbg_name = name;
    tasks_sync_init(&m_taskSync);
};

bool Mutex::lock()
{
    while (__atomic_test_and_set(&m_isLocked, __ATOMIC_RELEASE)) {
        TASK_ONLY tasks_sync_block(&m_taskSync);
    }

    return true;
}

bool Mutex::tryLock()
{
    if (__atomic_test_and_set(&m_isLocked, __ATOMIC_RELEASE)) {
        return false;
    }

    return true;
}

bool Mutex::unlock()
{
    __atomic_clear(&m_isLocked, __ATOMIC_RELEASE);
    TASK_ONLY tasks_sync_unblock(&m_taskSync);

    return true;
}
