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

private:
    /* both of these must be page aligned for anything to work right at all
       must be static for the section attributes to be applied */
    // kernel page directory
    [[gnu::section(".page_tables,\"aw\", @nobits#")]] static struct Arch::Memory::Directory m_directoryKernel;
    // page tables for the entire 32-bit address space
    [[gnu::section(".page_tables,\"aw\", @nobits#")]] static struct Arch::Memory::Table m_tables[ARCH_PAGE_TABLE_ENTRIES];

    Kernel()
        : Manager("kernel-virtual", m_directoryKernel, ARCH_DIR_ALIGN(KERNEL_START), ARCH_DIR_ALIGN_UP(KERNEL_END - KERNEL_START))
    {
        Interrupts::registerHandler(Interrupts::EXCEPTION_PAGE_FAULT, pageFaultException);
        Manager::initDirectory();
    }

    static void pageFaultException(struct registers* registers) { panic(registers); }

    void mapEarlyMemory();
    void mapKernelBinary();
    void mapKernelPageTable();
};

}
