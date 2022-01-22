/**
 * @file Mutex.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-30
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once

#include <stdint.h>
#include <Scheduler/tasks.hpp>

class Mutex {
public:
    /**
     * @brief Construct a new Mutex object
     *
     * @param name Mutex name (for debugging / printing)
     */
    Mutex(const char* name = nullptr);

    /**
     * @brief Aquire the mutex.
     *
     * @return int Returns true on success.
     */
    bool lock();

    /**
     * @brief Try to aquire the mutex and return immediately
     * if already locked.
     *
     * @return int Returns true on success.
     */
    bool tryLock();

    /**
     * @brief Release the mutex.
     *
     * @return int Returns true on success.
     */
    bool unlock();

private:
    bool m_isLocked;
    struct task_sync m_taskSync;
};
