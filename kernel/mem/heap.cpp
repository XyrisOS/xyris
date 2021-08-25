/**
 * @file heap.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Liballoc heap implementation
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <lib/errno.h>
#include <lib/mutex.hpp>
#include <mem/paging.hpp>
#include <stddef.h>

static Mutex lock("alloc");

#ifdef __cplusplus
extern "C" {
#endif

/*
 * lock the allocator.
 */
int liballoc_lock()
{
    int old_errno = errno;
    int result = lock.Lock();
    errno = old_errno;
    return result;
}

/*
 * unlock the allocator.
 */
int liballoc_unlock()
{
    int old_errno = errno;
    int result = lock.Unlock();
    errno = old_errno;
    return result;
}

/*
 * get a new page or pages.
 */
void* liballoc_alloc(unsigned int count)
{
    return get_new_page(count * PAGE_SIZE - 1);
}

/*
 * free a page or pages.
 */
int liballoc_free(void* page, unsigned int count)
{
    free_page(page, count * PAGE_SIZE - 1);
    return 0;
}

#ifdef __cplusplus
}
#endif
