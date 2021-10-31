/**
 * @file Physical.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Physical memory mapping
 * @version 0.1
 * @date 2021-10-28
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once

#include <arch/Memory.hpp>
#include <lib/Bitset.hpp>
#include <mem/MemorySection.hpp>
#include <meta/compiler.hpp>
#include <stddef.h>
#include <stdint.h>

namespace Memory::Physical {

class PhysicalManager {
public:
    PhysicalManager() = default;

    ALWAYS_INLINE void alloc(Section& sect)
    {
        (void)sect;
    }

    ALWAYS_INLINE void free(Section& sect)
    {
        (void)sect;
    }

    ALWAYS_INLINE void setFree(Section& sect)
    {
        (void)sect;
    }

    ALWAYS_INLINE void setUsed(Section& sect)
    {
        for (size_t i = 0; i < sect.pages(); i++) {
            setUsed(ADDRESS_TO_PAGE_IDX(sect.base()) + i);
        }
    }

    ALWAYS_INLINE bool isUsed(Section& sect)
    {
        (void)sect;
        return false;
    }

    ALWAYS_INLINE void alloc(uintptr_t addr)
    {
        (void)addr;
    }

    ALWAYS_INLINE void free(uintptr_t addr)
    {
        (void)addr;
    }

    ALWAYS_INLINE void setFree(uintptr_t addr)
    {
        m_memory.Reset(addr);
    }

    ALWAYS_INLINE void setUsed(uintptr_t addr)
    {
        m_memory.Set(addr);
    }

    ALWAYS_INLINE bool isUsed(uintptr_t addr)
    {
        (void)addr;
        return false;
    }

private:
    Bitset<MEM_BITMAP_SIZE> m_memory;
};

}
