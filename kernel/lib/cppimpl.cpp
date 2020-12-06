#include <mem/heap.hpp>
#include <sys/panic.hpp>

void *__attribute__((weak)) operator new(size_t size)
{
    return malloc(size);
}

void *__attribute__((weak)) operator new[](size_t size)
{
    return malloc(size);
}

void __attribute__((weak)) operator delete(void *ptr)
{
    free(ptr);
}

void __attribute__((weak)) operator delete[](void *ptr)
{
    free(ptr);
}

void __attribute__((weak)) operator delete(void *ptr, size_t size __attribute__((unused)))
{
    free(ptr);
}

void __attribute__((weak)) operator delete[](void *ptr, size_t size __attribute__((unused)))
{
    free(ptr);
}

extern "C" void __attribute__((weak)) __cxa_pure_virtual()
{
    PANIC("Attempt at calling a pure virtual function!");
}
