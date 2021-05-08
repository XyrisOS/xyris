#pragma once
#include <mem/paging.hpp>
#include <stddef.h>
#include <stdint.h>
#include <mem/MemoryTypes.hpp>

namespace Memory {

class MemoryRange {

public:
    static MemoryRange AlignUp(uintptr_t base, size_t size, MemoryType type);
    static MemoryRange AlignDown(uintptr_t base, size_t size, MemoryType type);
    uintptr_t Base();
    size_t Size();
    uintptr_t End();
    size_t Count();
    bool Empty();
    bool Aligned();
    bool Contains(uintptr_t addr);
    MemoryType Type();

private:
    uintptr_t base;
    size_t size;
    MemoryType type;
};

}
