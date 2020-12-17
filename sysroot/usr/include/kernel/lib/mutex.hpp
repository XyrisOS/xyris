/**
 * @file mutex.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-08-30
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */


#include <stdint.h>

typedef struct px_mutex {
    bool locked;
    px_mutex() {
        locked = false;
    };
} px_mutex_t;

/**
 * @brief Initializes a mutex for further use.
 * 
 * @param mutex Reference mutex
 * @return int Returns 0 on success and -1 on error.
 */
int px_mutex_init(px_mutex_t *mutex);
/**
 * @brief Destroys a mutex and removes it from memory.
 * 
 * @param mutex Reference mutex
 * @return int Returns 0 on success and -1 on error.
 */
int px_mutex_destroy(px_mutex_t *mutex);
/**
 * @brief Locks a provided mutex. This call will block
 * if the mutex is already locked.
 * 
 * @param mutex Reference mutex
 * @return int Returns 0 on success and -1 on error.
 */
int px_mutex_lock(px_mutex_t *mutex);
/**
 * @brief Attempts to lock a mutex. If the mutex is
 * currently locked then the function will return and
 * set errno accordingly.
 * 
 * @param mutex Reference mutex
 * @return int Returns 0 on success and -1 on error.
 */
int px_mutex_trylock(px_mutex_t *mutex);
/**
 * @brief Unlocks a mutex for others to use.
 * 
 * @param mutex Reference mutex
 * @return int Returns 0 on success and -1 on error.
 */
int px_mutex_unlock(px_mutex_t *mutex);
