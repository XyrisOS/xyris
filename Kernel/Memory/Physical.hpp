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
#include <Logger.hpp>
#include <stddef.h>
#include <stdint.h>

#define KADDR_TO_PHYS(addr) ((addr) - KERNEL_BASE)

namespace Memory::Physical {

class Manager {
public:
    Manager(Manager const&) = delete;
    void operator=(Manager const&) = delete;

    static Manager& the()
    {
        static Manager instance;
        return instance;
    }

    [[gnu::always_inline]] static void setFree(Section& sect)
    {
        for (size_t i = 0; i < sect.pages(); i++) {
            setFree(sect.base() + (i * ARCH_PAGE_SIZE));
        }
    }

    [[gnu::always_inline]] static void setUsed(Section& sect)
    {
        Logger::Debug(
            __func__,
            "0x%08zX-0x%08zX 0x%08zX [%zu] [%s]",
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

    [[gnu::always_inline]] static void setFree(Arch::Memory::Address addr)
    {
        the().m_memory.Clear(ADDRESS_TO_PAGE_IDX(addr));
    }

    [[gnu::always_inline]] static void setUsed(Arch::Memory::Address addr)
    {
        the().m_memory.Set(ADDRESS_TO_PAGE_IDX(addr));
    }

    [[gnu::always_inline]] static void setFree(uintptr_t addr)
    {
        the().m_memory.Clear(ADDRESS_TO_PAGE_IDX(addr));
    }

    [[gnu::always_inline]] static void setUsed(uintptr_t addr)
    {
        the().m_memory.Set(ADDRESS_TO_PAGE_IDX(addr));
    }

    [[gnu::always_inline]] static bool isFree(uintptr_t addr)
    {
        return the().m_memory.Test(ADDRESS_TO_PAGE_IDX(addr)) == 0;
    }

    [[gnu::always_inline]] static bool isFree(Section& sect)
    {
        for (size_t i = 0; i < sect.pages(); i++) {
            if (!isFree(sect.base()) + (i * ARCH_PAGE_SIZE)) {
                return false;
            }
        }

        return true;
    }

    [[gnu::always_inline]] static uintptr_t findNextFreePhysicalAddress()
    {
        return the().m_memory.FindFirstBit(false);
    }

private:
    Bitset<MEM_BITMAP_SIZE> m_memory;

    Manager()
        : m_memory(1)
    {
        // Always assume memory is reserved until proven otherwise
    }
};

}
