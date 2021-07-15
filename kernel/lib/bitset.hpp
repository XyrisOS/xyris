/**
 * @file bitset.hpp
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
#include <meta/compiler.hpp>

class Bitset {
public:
    typedef size_t bitmap_t;
    static ALWAYS_INLINE size_t TypeSize() { return sizeof(bitmap_t) * CHAR_BIT; }
    
    Bitset(void* buf, size_t size);
    ALWAYS_INLINE size_t Size() { return mapSize; }
    ALWAYS_INLINE void Set(size_t addr) { map[Index(addr)] |= 1UL << Offset(addr); }
    ALWAYS_INLINE bool Get(size_t addr) { return map[Index(addr)] >> Offset(addr) & 1; }
    ALWAYS_INLINE void Clear(size_t addr) { map[Index(addr)] &= ~(1UL << Offset(addr)); }
    size_t FindFirstBitClear();
    size_t FindFirstRangeClear(size_t count);

private:
    bitmap_t* map;
    size_t mapSize;
    ALWAYS_INLINE size_t Index(size_t bit) { return bit / TypeSize(); }
    ALWAYS_INLINE size_t Offset(size_t bit) { return bit % TypeSize(); }
};
