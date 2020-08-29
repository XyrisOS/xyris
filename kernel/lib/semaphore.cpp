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

// https://gcc.gnu.org/onlinedocs/gcc-8.3.0/gcc/_005f_005fatomic-Builtins.html

#include <lib/semaphore.hpp>

int px_sem_init(px_sem_t *sem, bool shared, uint32_t value) {

}

int px_sem_wait(px_sem_t *sem) {

}

int sem_trywait(px_sem_t *sem) {

}

int sem_timedwait(px_sem_t *sem, const uint32_t *usec) {

}

int px_sem_post(px_sem_t *sem) {

}

int px_sem_getval(px_sem_t *sem, uint32_t *val) {

}

int px_sem_destroy(px_sem_t *sem) {

}