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
#include "../Stubs/Memory/Arch.hpp"
#include <catch2/catch.hpp>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <ctime>

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
    return std::malloc(n);
}

void freePage(void* ptr, size_t n)
{
    (void)n;
    std::free(ptr);
}

} // !namespace Memory

#include <Memory/Heap.cpp>

} // !namespace Test

TEST_CASE("heap stress test", "[heap]")
{
    Test::Logger::Print("heap stress test\n");
    // Test::Logger::setLevel(Test::Logger::lTRACE);

    SECTION("push all then pop all")
    {
        Test::Logger::Print("\tpush all then pop all\n");
        const size_t count = 1024;
        const size_t allocSize = 1024;
        void* pointers[count];
        for (size_t pushAt = 0; pushAt < count; pushAt++) {
            void* ptr = Test::malloc(allocSize);
            REQUIRE(ptr != nullptr);

            for (size_t i = 0; i < pushAt; i++) {
                REQUIRE(pointers[i] != ptr);
            }

            std::memset(ptr, 'A', allocSize);
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

TEST_CASE("heap randomized stress test", "[heap]")
{
    const size_t maxBlocks = 100;
    const size_t maxSize = 1024 * 1024;
    const size_t maxTime = 1 * 60;

    struct block {
        uint8_t* data;
        uint32_t size;
        uint8_t key;
    };

    static struct block blocks[maxBlocks];
    static uint64_t totalMemory = 0;
    static uint32_t totalBlocks = 0;

    Test::Logger::Print("heap randomized stress test\n");
    Test::Logger::setLevel(Test::Logger::lTRACE);

    SECTION("randomized blocks")
    {
        Test::Logger::Print("\trandomized blocks\n");
        std::memset(blocks, 0, sizeof(struct block) * maxBlocks);

        int transactions = 0;
        time_t start_time = std::time(NULL);

        while (true) {
            int position = std::rand() % maxBlocks;

            int diff = std::time(NULL) - start_time;
            // Ensure the test doesn't take too long
            REQUIRE(diff < (maxTime));

            int tps = (++transactions) / (diff + 1);

            if (blocks[position].data == NULL) {
                blocks[position].size = std::rand() % maxSize;
                blocks[position].data = (uint8_t*)Test::malloc(blocks[position].size);
                blocks[position].key = std::rand() % 256;

                Test::Logger::Print("%lu left, %i tps : %lu, %i : %i: allocating %i bytes with %i key\n",
                    (maxTime - diff),
                    tps,
                    totalBlocks * 100 / maxBlocks,
                    (int)(totalMemory / (1024)),
                    position,
                    blocks[position].size,
                    blocks[position].key);

                if (blocks[position].data != NULL) {
                    totalMemory += blocks[position].size;
                    totalBlocks += 1;

                    for (int j = 0; j < blocks[position].size; j++) {
                        blocks[position].data[j] = blocks[position].key;
                    }
                }
            } else {
                for (int j = 0; j < blocks[position].size; j++) {
                    if (blocks[position].data[j] != blocks[position].key) {
                        Test::Logger::Print("%i: %p (%i bytes, position %i) %i != %i: ERROR! Memory not consistent\n",
                            position,
                            blocks[position].data,
                            blocks[position].size,
                            j,
                            blocks[position].data[j],
                            blocks[position].key);
                        REQUIRE(blocks[position].data[j] == blocks[position].key);
                    }
                }

                Test::Logger::Print("%lu left, %i tps : %lu, %i : %i: freeing %i bytes with %i key\n",
                    (maxTime - diff),
                    tps,
                    totalBlocks * 100 / maxBlocks,
                    (int)(totalMemory / (1024)),
                    position,
                    blocks[position].size,
                    blocks[position].key);

                Test::free(blocks[position].data);
                blocks[position].data = NULL;

                totalMemory -= blocks[position].size;
                totalBlocks -= 1;
            }
        }

        for (int i = 0; i < maxBlocks; i++) {
            if (blocks[i].data != NULL) {
                Test::free(blocks[i].data);
            }
            blocks[i].size = 0;
            blocks[i].key = 0;
        }

        Test::Logger::Print("%lu TPS, %lu%s USAGE\n", transactions / maxTime, totalBlocks * 100 / maxBlocks, "%");
    }
}
