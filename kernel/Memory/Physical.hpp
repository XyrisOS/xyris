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

#include <Arch/Memory.hpp>
#include <Library/Bitset.hpp>
#include <Memory/MemorySection.hpp>
#include <meta/compiler.hpp>
#include <stddef.h>
#include <stdint.h>

#define KADDR_TO_PHYS(addr) ((addr) - KERNEL_BASE)

namespace Memory::Physical {

class PhysicalManager {
public:
/*
    PhysicalManager()
        : m_memory(SIZE_MAX)
    {
        // Always assume memory is reserved until proven otherwise
    }
*/
    ALWAYS_INLINE void setFree(Section& sect)
    {
        for (size_t i = 0; i < sect.pages(); i++) {
            setFree(ADDRESS_TO_PAGE_IDX(sect.base()) + i);
        }
    }

    ALWAYS_INLINE void setUsed(Section& sect)
    {
        debugf(
            "0x%08zX-0x%08zX 0x%08zX [%zu] [%s]\n",
            sect.base(),
            sect.end(),
            sect.size(),
            sect.pages(),
            sect.typeString());
        // TODO: Optimize bitmap library to take number of bits to set
        for (size_t i = 0; i < sect.pages(); i++) {
            setUsed(ADDRESS_TO_PAGE_IDX(sect.base()) + i);
        }
    }

    ALWAYS_INLINE void setFree(Arch::Memory::Address addr)
    {
        if (!isFree(addr)) {
            m_memory.Reset(ADDRESS_TO_PAGE_IDX(addr));
        }
    }

    ALWAYS_INLINE void setUsed(Arch::Memory::Address addr)
    {
        if (isFree(addr)) {
            m_memory.Set(ADDRESS_TO_PAGE_IDX(addr));
        }
    }

    ALWAYS_INLINE void setFree(uintptr_t addr)
    {
        if (!isFree(addr)) {
            m_memory.Reset(ADDRESS_TO_PAGE_IDX(addr));
        }
    }

    ALWAYS_INLINE void setUsed(uintptr_t addr)
    {
        if (isFree(addr)) {
            m_memory.Set(ADDRESS_TO_PAGE_IDX(addr));
        }
    }

    ALWAYS_INLINE bool isFree(uintptr_t addr)
    {
        return m_memory.Test(ADDRESS_TO_PAGE_IDX(addr)) == 0;
    }

    ALWAYS_INLINE bool isFree(Section& sect)
    {
        // TODO: Optimize bitmap library to take number of bits to set
        for (size_t i = 0; i < sect.pages(); i++) {
            if (!isFree(ADDRESS_TO_PAGE_IDX(sect.base()) + i)) {
                return false;
            }
        }
        return true;
    }

    ALWAYS_INLINE uintptr_t findNextFreePhysicalAddress()
    {
        return m_memory.FindFirstBit(false);
    }

private:
    Bitset<MEM_BITMAP_SIZE> m_memory;
};

}
