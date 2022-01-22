/**
 * @file semaphore.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-28
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#include <Locking/Semaphore.hpp>

// Can't make this an inline function due to compiler errors with failure_memorder
// being too strong. Likely due to the fact that it doesn't know the value at compile time.
#define COMPARE_EXCHANGE(curVal, failure_memorder) __atomic_compare_exchange_n( \
    &m_count,                                                                   \
    &curVal,                                                                    \
    curVal - 1,                                                                 \
    false,                                                                      \
    __ATOMIC_RELEASE,                                                           \
    failure_memorder)

Semaphore::Semaphore(uint32_t val, bool share, const char* name)
    : m_isShared(share)
    , m_count(val)
{
    m_taskSync.dbg_name = name;
    tasks_sync_init(&m_taskSync);
}

bool Semaphore::wait()
{
    uint32_t curVal = count();
    do {
        while (curVal == 0) {
            TASK_ONLY tasks_sync_block(&m_taskSync);
            curVal = count();
        }
        // Fail using atomic relaxed because it may allow us to get to the "waiting" state faster.
    } while (!COMPARE_EXCHANGE(curVal, __ATOMIC_RELAXED));

    return true;
}

bool Semaphore::tryWait()
{
    uint32_t curVal = count();
    do {
        if (curVal == 0) {
            return false;
        }
        // We need to fail on an Atomic Acquire Release because it will fail less often (i.e. fewer loop iterations)
    } while (!COMPARE_EXCHANGE(curVal, __ATOMIC_ACQUIRE));

    return true;
}

bool Semaphore::timeWait(const uint32_t* usec)
{
    // TODO: Add the timer functionality here.
    (void)usec;
    uint32_t curVal = count();
    do {
        while (curVal == 0) {
            curVal = count();
        }
        // Fail using atomic relaxed because it may allow us to get to the "waiting" state faster.
    } while (!COMPARE_EXCHANGE(curVal, __ATOMIC_ACQUIRE));

    return true;
}

bool Semaphore::post()
{
    __atomic_fetch_add(&m_count, 1, __ATOMIC_RELEASE);
    TASK_ONLY tasks_sync_unblock(&m_taskSync);

    return true;
}

uint32_t Semaphore::count()
{
    uint32_t ret = 0;
    __atomic_load(&m_count, &ret, __ATOMIC_ACQUIRE);

    return ret;
}
