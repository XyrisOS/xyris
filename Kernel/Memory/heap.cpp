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
#include <Library/errno.hpp>
#include <Locking/Mutex.hpp>
#include <Memory/heap.hpp>
#include <Memory/paging.hpp>
#include <stddef.h>

static Mutex lock("alloc");

extern "C" {

int liballoc_lock()
{
    return (lock.lock() ? 0 : 1);
}

int liballoc_unlock()
{
    return (lock.unlock() ? 0 : 1);
}

void* liballoc_alloc(unsigned int count)
{
    return Memory::newPageMustSucceed(count * ARCH_PAGE_SIZE - 1);
}

int liballoc_free(void* page, unsigned int count)
{
    Memory::freePage(page, count * ARCH_PAGE_SIZE - 1);
    return 0;
}

}

void* operator new(size_t size)
{
    return malloc(size);
}

void* operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void* p)
{
    free(p);
}

void operator delete[](void* p)
{
    free(p);
}

void operator delete(void* p, long unsigned int)
{
    free(p);
}

void operator delete[](void* p, long unsigned int)
{
    free(p);
}
