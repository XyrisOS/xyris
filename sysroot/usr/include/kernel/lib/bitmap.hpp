/**
 * @file bitmap.hpp
 * @author Micah Switzer (mswitzer@cedarville.edu)
 *         Keeton Feasvel (keetonfeavel@cedarville.edu)
 * @brief A basic bitmap implementation
 * @version 0.3
 * @date 2020-07-08
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 *
 */
#pragma once

#include <stddef.h>
#include <limits.h>

class Bitmap {
public:
    Bitmap(void* buf, size_t size);
    void Set(size_t addr);
    bool Get(size_t addr);
    void Clear(size_t addr);
    size_t FindFirstBitClear();
    size_t FindFirstRangeClear(size_t count);

private:
    typedef size_t bitmap_t;
    bitmap_t* map;
    size_t mapSize;
    inline size_t Size();
    inline size_t TypeSize();
    inline size_t Index(size_t bit);
    inline size_t Offset(size_t bit);
};
