/**
 * @file Kernel.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Kernel virtual memory manager
 * @version 0.1
 * @date 2022-02-20
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once

#include <Arch/Memory.hpp>
#include <Library/Bitset.hpp>
#include <Locking/Mutex.hpp>
#include <Logger.hpp>
#include <Memory/MemorySection.hpp>
#include <Memory/Physical.hpp>
#include <Memory/Virtual.hpp>
#include <Panic.hpp>
#include <Support/sections.hpp>
#include <stddef.h>
#include <stdint.h>

namespace Memory::Virtual {

class Kernel : Manager {
public:
    Kernel(Kernel const&) = delete;
    void operator=(Kernel const&) = delete;

    static Kernel& the()
    {
        static Kernel instance;
        return instance;
    }

    /**
     * @brief Get the physical address of the kernel virtual memory page directory.
     *
     * @return uintptr_t Physical address of the kernel virtual memory page directory.
     */
    static uintptr_t getPageDirectoryPhysicalAddress() { return KADDR_TO_PHYS((uintptr_t)&the().m_directoryKernel); }

    /**
     * @brief Determine if an address is mapped into the virtual address space.
     *
     * @param addr Address to be examined.
     * @return true Address is mapped into the virtual address space.
     * @return false Address is not mapped into the virtual address space.
     */
    static bool isAddressMapped(uintptr_t addr) { return the().m_memory[addr >> ARCH_PAGE_TABLE_ENTRY_SHIFT]; }

private:
    Bitset<MEM_BITMAP_SIZE> m_memory;

    /* both of these must be page aligned for anything to work right at all
       must be static for the section attributes to be applied */
    // kernel page directory
    [[gnu::section(".page_tables,\"aw\", @nobits#")]] static struct Arch::Memory::Directory m_directoryKernel;
    // page tables for the entire 32-bit address space
    [[gnu::section(".page_tables,\"aw\", @nobits#")]] static struct Arch::Memory::Table m_tables[ARCH_PAGE_TABLE_ENTRIES];

    Kernel()
        : Manager("kernel-virtual", m_directoryKernel, KERNEL_START, ADDRESS_SPACE_SIZE - KERNEL_START)
    {
        Interrupts::registerHandler(Interrupts::EXCEPTION_PAGE_FAULT, pageFaultException);
    }

    static void pageFaultException(struct registers* registers) { panic(registers); }

    static void initPhysical(MemoryMap* map);
    static void initDirectory();
    static void mapEarlyMemory();
    static void mapKernelBinary();
    static void mapKernelPageTable();
};

}
