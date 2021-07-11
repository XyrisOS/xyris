#include <lib/bitset.hpp>
#include <sys/panic.hpp>
#include <stdint.h>

Bitset::Bitset(void* buf, size_t size)
    : map((bitmap_t*)buf)
    , mapSize(size)
{
    // Ensure pointer alignment
    if ((uintptr_t)buf % alignof(bitmap_t) != 0) {
        PANIC("Unaligned bitmap pointer");
    }
}

size_t Bitset::FindFirstBitClear()
{
    for (size_t i = 0; i < mapSize; i++) {
        if (!((map[Index(i)] >> Offset(i)) & 1)) return i;
    }
    return SIZE_MAX;
}

size_t Bitset::FindFirstRangeClear(size_t count)
{
    size_t check_lo, check_hi, check, idx, ofst;
    size_t mask = ((size_t)1 << count) - (size_t)1;
    for (size_t i = 0UL; i < mapSize - count; i++) {
        idx = Index(i);
        ofst = Offset(i);
        check_lo = map[idx] >> ofst;
        check_hi = ofst ? map[idx + 1] << (TypeSize() - ofst) : 0;
        check = check_lo | check_hi;
#ifdef TESTING
        size_t masked = check & mask;
        printf("i = %zd, idx = 0x%08zx, ofst = 0x%08zx, check_lo = 0x%08zx, "
               "check_hi = 0x%08zx, check = 0x%08zx, masked = 0x%08zx\n",
                i, idx, ofst, check_lo, check_hi, check, masked);
#endif
        if (!(check & mask)) return i;
    }
    return SIZE_MAX;
}
