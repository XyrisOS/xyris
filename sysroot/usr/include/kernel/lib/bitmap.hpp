/**
 * @file bitmap.hpp
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @brief A basic bitmap implementation
 * @version 0.3
 * @date 2020-07-08
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 *
 */
#pragma once

#include <stddef.h>

typedef size_t bitmap_t;

// the number of bits in a byte
#define BYTE_SIZE 8
#define SIZE_T_MAX_VALUE ((size_t)0 -1)

#define BITS_PER_BITMAP_T (sizeof(bitmap_t) * BYTE_SIZE)

#define INDEX_FROM_BIT(a)   ((a) / BITS_PER_BITMAP_T)
#define OFFSET_FROM_BIT(a)  ((a) % BITS_PER_BITMAP_T)

#define SET_BIT_IN_MAP(bmp, addr) (bmp)[INDEX_FROM_BIT((addr))] \
    |= 1UL << OFFSET_FROM_BIT((addr))
#define UNSET_BIT_IN_MAP(bmp, addr) (bmp)[INDEX_FROM_BIT((addr))] \
    &= ~(1UL << OFFSET_FROM_BIT((addr)))

#define BITMAP_SIZE(NUM_BITS) ((NUM_BITS) / BITS_PER_BITMAP_T)

static inline void bitmap_set_bit(bitmap_t *bitmap, size_t index) {
    SET_BIT_IN_MAP(bitmap, index);
}

static inline bool bitmap_get_bit(bitmap_t *bitmap, size_t index) {
    return ((bitmap)[INDEX_FROM_BIT((index))] >> OFFSET_FROM_BIT((index))) & 1;
}

static inline void bitmap_clear_bit(bitmap_t *bitmap, size_t index) {
    UNSET_BIT_IN_MAP(bitmap, index);
}

size_t bitmap_find_first_bit_clear(bitmap_t *bitmap, size_t start, size_t size);
size_t bitmap_find_first_range_clear(bitmap_t *bitmap, size_t start, size_t size, size_t count);
