/**
 * @file mutex.hpp
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
#include <sys/tasks.hpp>

class Mutex {
public:
    /**
     * @brief Construct a new Mutex object
     *
     * @param name Mutex name (for debugging / printing)
     */
    Mutex(const char* name = nullptr);
    /**
     * @brief Destroys a mutex and removes it from memory.
     *
     */
    ~Mutex();
    /**
     * @brief Locks a provided mutex. This call will block
     * if the mutex is already locked.
     *
     * @return int Returns 0 on success and -1 on error.
     */
    int Lock();
    /**
     * @brief Attempts to lock a mutex. If the mutex is
     * currently locked then the function will return and
     * set errno accordingly.
     *
     * @return int Returns 0 on success and -1 on error.
     */
    int Trylock();
    /**
     * @brief Unlocks a mutex for others to use.
     *
     * @return int Returns 0 on success and -1 on error.
     */
    int Unlock();

private:
    bool locked;
    struct task_sync task_sync;
};
