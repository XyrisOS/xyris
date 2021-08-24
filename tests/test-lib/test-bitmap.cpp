/**
 * @file test-bitmap.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Bitset library unit tests
 * @version 0.1
 * @date 2021-07-09
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <catch2/catch.hpp>
#include <test-helpers/test-panic.hpp>
#include <lib/bitset.cpp>

#define TEST_BITMAP_SIZE 4096 / (sizeof(size_t) * CHAR_BIT)
static size_t bitmapArray[TEST_BITMAP_SIZE];

TEST_CASE( "Set", "[bitmap]" ) {
    Bitset map = Bitset(bitmapArray, sizeof(bitmapArray));
    (void)map;
}
