#pragma once
#include <mem/paging.hpp>
#include <stddef.h>
#include <stdint.h>

enum MemoryRangeType {
    MemoryUsable = 0,
    MemoryReserved,
    MemoryAcpiReclaimable,
    MemoryAcpiNVS,
    MemoryBad,
    MemoryBootloader,
    MemoryKernel,
};

class MemoryRange {
public:
    static MemoryRange AlignUp(uintptr_t base, size_t size, MemoryRangeType type);
    static MemoryRange AlignDown(uintptr_t base, size_t size, MemoryRangeType type);
    uintptr_t Base();
    size_t Size();
    uintptr_t End();
    size_t Count();
    bool Empty();
    bool Aligned();
    bool Contains(uintptr_t addr);
    MemoryRangeType Type();
private:
    uintptr_t _base;
    size_t _size;
    MemoryRangeType _type;
};
