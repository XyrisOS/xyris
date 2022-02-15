#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

//#define debugf(...) printf("DEBUG: " __VA_ARGS__)
namespace Test {
#include <Memory/Heap.hpp>
namespace Memory {
void *newPage(unsigned int n)
{
    return ::malloc(n);
}
void freePage(void *ptr, unsigned int n)
{
    (void)n;
    ::free(ptr);
}
}
}

static void *testMalloc(unsigned int count)
{
    printf("MALLOC(0x%08x)\n", count);
    void *res = Test::malloc(count);
    //printf("%p\n", res);
    return res;
}

static void *testRealloc(void *ptr, unsigned int new_size)
{
    printf("REALLOC(%p, 0x%08x) = ", ptr, new_size);
    void *res = Test::realloc(ptr, new_size);
    printf("%p\n", res);
    return res;
}

static void testFree(void *ptr)
{
    printf("FREE(%p)\n", ptr);
    Test::free(ptr);
}

typedef struct {
    void *ptr;
    size_t len;
} region;

static size_t pushAt = 0, popAt = 0;
static region regions[1024];

static void checkDuplicate(void *ptr)
{
    for (size_t i = 0; i < pushAt; i++) {
        if (regions[i].ptr == ptr) {
            printf("DUPLICATE ADDR RETURNED (first at %zu, now at %zu) [%p]\n", i, pushAt, ptr);
            exit(1);
        }
    }
}

static void *pushRegion(size_t size)
{
    void *ptr = testMalloc(size);
    checkDuplicate(ptr);
    regions[pushAt++] = {
        .ptr = ptr,
        .len = size,
    };
    if (ptr)
        memset(ptr, 'A', size);
    return ptr;
}

static void popRegion()
{
    testFree(regions[popAt++].ptr);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("XYRIS HEAP TESTS");

    while (pushAt < 1024)
        pushRegion(1024);

    while (popAt < 1024)
        popRegion();

    puts("DONE");
    return 0;
}
