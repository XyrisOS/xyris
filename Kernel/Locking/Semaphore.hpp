/**
 * @file Semaphore.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-28
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once

#include <stdint.h>
#include <Scheduler/tasks.hpp>

class Semaphore {
public:
    /**
     * @brief Initializes a semaphore struct using the values
     * defined. Semaphores are used in order to maintain mutual
     * exclusion when multiple threads need to access a particular
     * variable.
     *
     * @param val Initial value
     * @param share Allow semaphore to be shared across threads
     * @param name Semaphore name (for debugging / printing)
     */
    Semaphore(uint32_t val, bool share, const char* name = nullptr);

    /**
     * @brief Wait for the semaphore to become available.
     * @return int Returns true on success.
     */
    bool wait();

    /**
     * @brief Check if the semaphore is available. If unavailable, return immediately,
     * otherwise wait on the semaphore.
     *
     * @return int Returns 0 on success.
     */
    bool tryWait();

    /**
     * @brief Wait on the semaphore for a set duration of time.
     *
     * @param usec Microseconds to wait until returning if unsuccessful.
     * @return int Returns true on success.
     */
    bool timeWait(const uint32_t* usec);

    /**
     * @brief Post to the semaphore.
     *
     * @return int Returns true on success.
     */
    bool post();

    /**
     * @brief Atomically get the semaphore's current counter value.
     *
     * @return uint32_t Returns the semaphore count.
     */
    uint32_t count();

private:
    bool m_isShared;
    uint32_t m_count;
    struct task_sync m_taskSync;
};
