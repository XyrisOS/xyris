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
#include <stddef.h>
#include <stdint.h>

template<size_t t_num_bits>
class Bitset {
public:
    Bitset() = default;

    Bitset(bool defaultValue)
        : m_numBits(t_num_bits)
        , m_count(0)
    {
        for (size_t i = 0; i < sizeof(m_bitset) / sizeof(*m_bitset); i++) {
            m_bitset[i] = (defaultValue ? SIZE_MAX : 0);
        }
    }

    [[gnu::always_inline]] size_t Size() { return m_numBits; }
    [[gnu::always_inline]] size_t Count() { return m_count; }

    [[gnu::always_inline]] void Set(size_t pos)
    {
        m_count++;
        m_bitset[Index(pos)] |= 1UL << Offset(pos);
    }

    [[gnu::always_inline]] void Clear(size_t pos)
    {
        m_count--;
        m_bitset[Index(pos)] &= ~(1UL << Offset(pos));
    }

    [[gnu::always_inline]] void Flip(size_t pos) { Test(pos) ? Clear(pos) : Set(pos); }
    [[gnu::always_inline]] bool Test(size_t pos) { return m_bitset[Index(pos)] >> Offset(pos) & 1; }
    [[gnu::always_inline]] bool Any() { return m_count > 0; }
    [[gnu::always_inline]] bool None() { return m_count == 0; }
    [[gnu::always_inline]] bool All() { return m_count == m_numBits; }

    /**
     * @brief Return the value of the bit at the given position
     * Same functionality as Test() as an operator
     * This operator cannot be used to set a bit, only test
     *
     * @param pos Position to be tested
     * @return bool Bit value
     */
    [[gnu::always_inline]] bool operator[](size_t pos) { return Test(pos); }

    /**
     * @brief Finds and returns the position of the first clear bit.
     *
     * @param isSet If true, find the first bit that is set.
     * @return size_t Position of the first bit with desired polarity.
     * If all bits are polarized, SIZE_MAX is returned.
     */
    [[gnu::always_inline]] size_t FindFirstBit(bool isSet)
    {
        for (size_t i = 0; i < t_num_bits; i++) {
            if (Test(i) == isSet) {
                return i;
            }
        }

        return Bitset::npos;
    }

    /**
     * @brief Finds a range of `count` clear bits and returns the starting position.
     *
     * @param count Number of clear bits desired
     * @param isSet If true, find the first range of `count` bits that are set.
     * @return size_t Position of the first bit with desired range and polarity.
     * If all bits are polarized, SIZE_MAX is returned.
     */
    [[gnu::always_inline]] size_t FindFirstRange(size_t count, bool isSet)
    {
        size_t checkLow, checkHigh, check, idx, offset;
        size_t mask = ((size_t)1 << count) - (size_t)1;
        for (size_t i = 0UL; i < t_num_bits - count; i++) {
            idx = Index(i);
            offset = Offset(i);
            checkLow = m_bitset[idx] >> offset;
            checkHigh = offset ? m_bitset[idx + 1] << (TypeSize() - offset) : 0;
            check = checkLow | checkHigh;

            if ((check & mask) == isSet) {
                return i;
            }
        }

        return Bitset::npos;
    }

    /**
     * @brief When used as a value for a position, indicates the last position in the bitset.
     * When provided as a return value, it indicates no matches.
     *
     */
    static const size_t npos = SIZE_MAX;

private:
    size_t m_numBits;
    size_t m_count;
    size_t m_bitset[t_num_bits / (CHAR_BIT * sizeof(size_t))];

    [[gnu::always_inline]] size_t TypeSize() { return sizeof(size_t) * CHAR_BIT; }

    /**
     * @brief Index into the array of size_t's (size_t containing bit @ position)
     *
     * @param position Position (index) of desired bit
     * @return size_t Index of size_t in m_bitset containing bit at position
     */
    [[gnu::always_inline]] size_t Index(size_t position) { return position / TypeSize(); }

    /**
     * @brief Number of bits to offset in size_t to get desired bit at position
     *
     * @param position Position (index) of desired bit
     * @return size_t Offset into size_t for desired bit
     */
    [[gnu::always_inline]] size_t Offset(size_t position) { return position % TypeSize(); }
};
