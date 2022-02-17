/**
 * @file Heap.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2022-02-16
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#include <catch2/catch.hpp>
#include <cstdarg>
#include "../Stubs/Memory/Arch.hpp"

namespace Test {

#include <Library/rand.cpp>
#include <Library/string.cpp>

namespace RTC {

uint64_t getEpoch(void) { return 0; }

} // !namespace RTC

namespace Memory {

void* newPage(size_t n)
{
    // size - 1 is passed due to a Xyris bug
    n += 1;
    return ::malloc(n);
}

void freePage(void* ptr, size_t n)
{
    (void)n;
    ::free(ptr);
}

} // !namespace Memory

#include <Memory/Heap.cpp>

} // !namespace Test

static void* testMalloc(unsigned long count)
{
    return Test::malloc(count);
}

static void* testRealloc(void* ptr, unsigned long new_size)
{
    return Test::realloc(ptr, new_size);
}

static void testFree(void* ptr)
{
    Test::free(ptr);
}

typedef struct {
    void* ptr;
    size_t len;
} region;

static size_t pushAt = 0;
static size_t popAt = 0;
static region regions[1024];

static void checkDuplicate(void* ptr)
{
    for (size_t i = 0; i < pushAt; i++) {
        REQUIRE(regions[i].ptr != ptr);
    }
}

static void* pushRegion(size_t size)
{
    void* ptr = testMalloc(size);
    checkDuplicate(ptr);
    regions[pushAt++] = {
        .ptr = ptr,
        .len = size,
    };

    if (ptr) {
        memset(ptr, 'A', size);
    }

    return ptr;
}

static void popRegion()
{
    testFree(regions[popAt++].ptr);
}

TEST_CASE("heap stress test", "[heap]")
{
    SECTION("constructor")
    {
        const size_t size = 1024;
        while (pushAt < size) {
            pushRegion(size);
        }

        while (popAt < size) {
            popRegion();
        }
    }
}
