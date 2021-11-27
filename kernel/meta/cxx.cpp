/**
 * @file cxx.cpp
 * @author Micah Switzer <mswitzer@cedarville.edu>
 * @brief C++ dynamic memory management functions
 * @version 0.1
 * @date 2021-07-25
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 * References:
 *         https://wiki.osdev.org/C++
 */
#include <Memory/heap.hpp>

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
