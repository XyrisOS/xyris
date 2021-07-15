/**
 * @file semaphore.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-28
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 */
#pragma once

#include <stdint.h>
#include <sys/tasks.hpp>

class Semaphore {
public:
    /**
     * @brief Initializes a semaphore struct using the values
     * defined. Semaphores are used in order to maintain mutual
     * exclusion when multiple threads need to access a particular
     * variable.
     *
     * @param val Initialized value (how many times must the semaphore
     * be accessed at the beginning before locking?)
     * @param share Boolean value indicating whether the semaphore
     * should be shared amongst threads or will be used within one thread
     * exclusively.
     * @param name Name of the mutex (for debugging tasks)
     * @return int Returns 0 if the semaphore was initialized successfully.
     */

    Semaphore(uint32_t val, bool share, const char* name = nullptr);
    /**
     * @brief Destroys a semaphore by removing it from memory.
     *
     */
    ~Semaphore();
    /**
     * @brief Waiting on a semaphore decrements the count value. When a
     * count value is 0, the semaphore is locked and the process or thread
     * must sleep and wait until the semaphore is unlocked by calling post.
     *
     * @return int Returns 0 on success and -1 on failure
     * When an error occurs errno is set.
     */
    int Wait();
    /**
     * @brief Functionally the same as sem_wait except that instead of
     * blocking when the semaphore is locked, errno is set instead.
     *
     * @return int Returns 0 on success and -1 on failure.
     * When an error occurs errno is set.
     */
    int Trywait();
    /**
     * @brief Functionally the same as sem_wait but with a timeout. This
     * timeout does not mean that within the given period of time the
     * semaphore will be unlocked but rather that after the timeout period
     * the thread or process will unblock and may resume execution without
     * access to the semaphore's reference variable.
     *
     * @param usec Microseconds to wait until resuming execution without
     * access to the semaphore's intended reference variable.
     * @return int Returns 0 on success and -1 on failure.
     * When an error occurs errno is set.
     */
    int Timedwait(const uint32_t* usec);
    /**
     * @brief Increments the semaphore's counter. This is used to indicate
     * that the thread or process is done utilizing the reference variable
     * and that it is available for use.
     *
     * @return int Returns 0 on success and -1 on failure.
     * When an error occurs errno is set.
     */
    int Post();
    /**
     * @brief Gets the counter value from a semaphore structure.
     *
     * @param val Pointer to the semaphore value variable
     * @return int Returns 0 on success and -1 on failure.
     * When an error occurs errno is set.
     */
    int Count(uint32_t* val);

private:
    bool shared;
    uint32_t count;
    tasks_sync_t task_sync;
};
