/**
 * @file Bitset.hpp
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @author Keeton Feasvel (keetonfeavel@cedarville.edu)
 * @brief A basic bitmap implementation
 * @version 0.3
 * @date 2020-07-08
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once

#include <Arch/Arch.hpp>
#include <Library/string.hpp>
#include <limits.h>
#include <meta/compiler.hpp>
#include <stddef.h>
#include <stdint.h>

template<size_t S>
class Bitset {
public:
    /**
     * @brief Construct a new Bitset object and initalize
     * the bitmap to zero.
     *
     */
    Bitset()
    {
        memset(&map, 0, S);
    }

    /**
     * @brief Construct a new Bitset object and initialize
     * the bitmap with the desired value
     *
     * @param val Initialization value
     */
    Bitset(size_t val)
    {
        for (size_t i = 0; i < S; i++)
            map[i] = val;
    }

    /**
     * @brief Returns the size of the bitset in bytes
     *
     * @return size_t Size of the bitset in bytes
     */
    ALWAYS_INLINE size_t Size()
    {
        return S;
    }

    /**
     * @brief Set the bit for a given position
     *
     * @param pos Target bit to be set
     */
    ALWAYS_INLINE void Set(size_t pos)
    {
        map[Index(pos)] |= 1UL << Offset(pos);
    }

    /**
     * @brief Reset (clear) the bit at the given position
     *
     * @param pos Target bit to be reset
     */
    ALWAYS_INLINE void Reset(size_t pos)
    {
        map[Index(pos)] &= ~(1UL << Offset(pos));
    }

    /**
     * @brief Flip the bit at the given position
     *
     * @param pos Target bit to be flipped
     */
    ALWAYS_INLINE void Flip(size_t pos)
    {
        Test(pos) ? Reset(pos) : Set(pos);
    }

    /**
     * @brief Return the value of the bit at the given position
     *
     * @param pos Position to be tested
     * @return bool Bit value
     */
    ALWAYS_INLINE bool Test(size_t pos)
    {
        return map[Index(pos)] >> Offset(pos) & 1;
    }

    /**
     * @brief Return the value of the bit at the given position
     * Same functionality as Test() as an operator
     * This operator cannot be used to set a bit, only test
     *
     * @param pos Position to be tested
     * @return bool Bit value
     */
    ALWAYS_INLINE bool operator[](size_t pos)
    {
        return Test(pos);
    }

    /**
     * @brief Finds and returns the position of the first clear bit.
     *
     * @param isSet If true, find the first bit that is set.
     * @return size_t Position of the first bit with desired polarity.
     * If all bits are polarized, SIZE_MAX is returned.
     */
    ALWAYS_INLINE size_t FindFirstBit(bool isSet)
    {
        for (size_t i = 0; i < S; i++) {
            if (Test(i) == isSet)
                return i;
        }
        return SIZE_MAX;
    }

    /**
     * @brief Finds a range of `count` clear bits and returns the starting position.
     *
     * @param count Number of clear bits desired
     * @param isSet If true, find the first range of `count` bits that are set.
     * @return size_t Position of the first bit with desired range and polarity.
     * If all bits are polarized, SIZE_MAX is returned.
     */
    ALWAYS_INLINE size_t FindFirstRange(size_t count, bool isSet)
    {
        size_t checkLow, checkHigh, check, idx, offset;
        size_t mask = ((size_t)1 << count) - (size_t)1;
        for (size_t i = 0UL; i < S - count; i++) {
            idx = Index(i);
            offset = Offset(i);
            checkLow = map[idx] >> offset;
            checkHigh = offset ? map[idx + 1] << (TypeSize() - offset) : 0;
            check = checkLow | checkHigh;

            if ((check & mask) == isSet)
                return i;
        }
        return SIZE_MAX;
    }

private:
    size_t map[S];
    ALWAYS_INLINE size_t TypeSize()
    {
        return sizeof(size_t) * CHAR_BIT;
    }

    ALWAYS_INLINE size_t Index(size_t bit)
    {
        return bit / TypeSize();
    }

    ALWAYS_INLINE size_t Offset(size_t bit)
    {
        return bit % TypeSize();
    }
};
