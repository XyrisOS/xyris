#include <lib/bitmap.hpp>

size_t bitmap_find_first_range_clear(bitmap_t *bitmap, size_t size, size_t count) {
    size_t bm, check_lo, check_hi, check, masked, idx, ofst;
    size_t mask = ((size_t)1 << count) - (size_t)1;
    for (size_t i = 0UL; i < size - count; i++) {
        idx = INDEX_FROM_BIT(i);
        ofst = OFFSET_FROM_BIT(i);
        check_lo = bitmap[idx] >> ofst;
        check_hi = ofst ? bitmap[idx + 1] << (BITS_PER_BITMAP_T - ofst) : 0;
        check = check_lo | check_hi;
        masked = check & mask;
        if (!(check & mask)) return i;
    }
    return SIZE_T_MAX_VALUE;
}

size_t bitmap_find_first_bit_clear(bitmap_t *bitmap, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (!((bitmap[INDEX_FROM_BIT(i)] >> OFFSET_FROM_BIT(i)) & 1)) return i;
    }
    return SIZE_T_MAX_VALUE;
}
