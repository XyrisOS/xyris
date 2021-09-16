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
#include <mem/heap.hpp>
#include <mem/paging.hpp>
#include <stddef.h>

static Mutex lock("alloc");

extern "C" {

int liballoc_lock()
{
    int old_errno = errno;
    int result = lock.Lock();
    errno = old_errno;
    return result;
}

int liballoc_unlock()
{
    int old_errno = errno;
    int result = lock.Unlock();
    errno = old_errno;
    return result;
}

// TODO: Move PAGE_SIZE to architecture headers?
void* liballoc_alloc(unsigned int count)
{
    return Paging::newPage(count * ARCH_PAGE_SIZE - 1);
}

int liballoc_free(void* page, unsigned int count)
{
    Paging::freePage(page, count * ARCH_PAGE_SIZE - 1);
    return 0;
}

}
