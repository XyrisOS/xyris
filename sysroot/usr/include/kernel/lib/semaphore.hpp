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

#include <stdint.h>

typedef struct px_semaphore
{
    bool shared;
    uint32_t count;
} px_sem_t;

/**
 * @brief Initializes a semaphore struct using the values
 * defined. Semaphores are used in order to maintain mutual
 * exclusion when multiple threads need to access a particular
 * variable.
 * 
 * @param sem Pointer to the semaphore structure to be used. 
 * @param shared Boolean value indicating whether the semaphore
 * will be shared amongst threads or will be used within one thread
 * exclusively.
 * @param value Initialized value (how many times must the semaphore
 * be accessed at the beginning before locking?) 
 * @return int Returns 0 if the semaphore was initialized successfully.
 */ 
int px_sem_init(px_sem_t *sem, bool shared, uint32_t value);
/**
 * @brief Waiting on a semaphore decrements the count value. When a
 * count value is 0, the semaphore is locked and the process or thread
 * must sleep and wait until the semaphore is unlocked by calling post.
 * 
 * @param sem Reference semaphore
 * @return int Returns 0 on success and -1 on failure
 * When an error occurs errno is set.
 */
int px_sem_wait(px_sem_t *sem);
/**
 * @brief Functionally the same as px_sem_wait except that instead of
 * blocking when the semaphore is locked, errno is set instead.
 * 
 * @param sem Reference semaphore
 * @return int Returns 0 on success and -1 on failure.
 * When an error occurs errno is set.
 */
int sem_trywait(px_sem_t *sem);
/**
 * @brief Functionally the same as px_sem_wait but with a timeout. This
 * timeout does not mean that within the given period of time the
 * semaphore will be unlocked but rather that after the timeout period
 * the thread or process will unblock and may resume execution without
 * access to the semaphore's reference variable.
 * 
 * @param sem Reference semaphore
 * @param usec Microseconds to wait until resuming execution without
 * access to the semaphore's intended reference variable.
 * @return int Returns 0 on success and -1 on failure.
 * When an error occurs errno is set.
 */
int sem_timedwait(px_sem_t *sem, const uint32_t *usec);
/**
 * @brief Increments the semaphore's counter. This is used to indicate
 * that the thread or process is done utilizing the reference variable
 * and that it is available for use.
 * 
 * @param sem Reference semaphore
 * @return int Returns 0 on success and -1 on failure.
 * When an error occurs errno is set.
 */
int px_sem_post(px_sem_t *sem);
/**
 * @brief Gets the counter value from a semaphore structure.
 * 
 * @param sem Reference semaphore
 * @param val 
 * @return int Returns 0 on success and -1 on failure.
 * When an error occurs errno is set.
 */
uint32_t px_sem_getval(px_sem_t *sem, uint32_t *val);
/**
 * @brief Destroys a semaphore by removing it from memory.
 * 
 * @param sem Reference semaphore
 * @return int Returns 0 on success and -1 on failure.
 * When an error occurs errno is set.
 */
int px_sem_destroy(px_sem_t *sem);
