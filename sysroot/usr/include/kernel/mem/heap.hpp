/**
 * @file heap.hpp
 * @author Goswin von Brederlow (goswin-v-b@web.de)
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-22
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 * Code shamelessly taken from the OSDev Wiki. The article
 * can be found at the link below.
 * 
 * https://wiki.osdev.org/User:Mrvn/LinkedListBucketHeapImplementation
 */
#pragma once

#include <stddef.h>

extern "C"
{
/**
 * @brief This function is supposed to lock the memory data structures. It
 * could be as simple as disabling interrupts or acquiring a spinlock.
 * It's up to you to decide.
 *
 * @return 0 if the lock was acquired successfully. Anything else is
 * failure.
 */
int liballoc_lock();

/**
 * @brief This function unlocks what was previously locked by the liballoc_lock
 * function.  If it disabled interrupts, it enables interrupts. If it
 * had acquiried a spinlock, it releases the spinlock. etc.
 *
 * @return 0 if the lock was successfully released.
 */
int liballoc_unlock();

/**
 * @brief This is the hook into the local system which allocates pages. It
 * accepts an integer parameter which is the number of pages
 * required.  The page size was set up in the liballoc_init function.
 *
 * @return NULL if the pages were not allocated.
 * @return A pointer to the allocated memory.
 */
void* liballoc_alloc(unsigned int);

/**
 * @brief This frees previously allocated memory. The void* parameter passed
 * to the function is the exact same value returned from a previous
 * liballoc_alloc call.
 *
 * The integer value is the number of pages to free.
 *
 * @return 0 if the memory was successfully freed.
 */
int liballoc_free(void*, unsigned int);

extern void* malloc(size_t);
extern void* realloc(void*, size_t);
extern void* calloc(size_t, size_t);
extern void  free(void*);
}
