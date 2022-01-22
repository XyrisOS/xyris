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
