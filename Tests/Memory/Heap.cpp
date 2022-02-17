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

TEST_CASE("heap stress test", "[heap]")
{
    srand(time(NULL));

    SECTION("push all then pop all")
    {
        const size_t count = 1024;
        const size_t allocSize = 1024;
        void* pointers[count];
        for (size_t pushAt = 0; pushAt < count; pushAt++) {
            void* ptr = Test::malloc(allocSize);
            REQUIRE(ptr != nullptr);

            for (size_t i = 0; i < pushAt; i++) {
                REQUIRE(pointers[i] != ptr);
            }

            memset(ptr, 'A', allocSize);
            pointers[pushAt] = ptr;
        }

        for (size_t popAt = 0; popAt < count; popAt++) {
            void* ptr = pointers[popAt];
            for (size_t i = 0; i < allocSize; i++) {
                REQUIRE(((char*)ptr)[i] == 'A');
            }

            Test::free(ptr);
        }
    }
}
