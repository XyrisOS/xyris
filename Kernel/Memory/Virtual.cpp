#include "Virtual.hpp"
#include <Panic.hpp>

namespace Memory::Virtual {

void* Manager::map(size_t size, enum MapFlags flags)
{
    (void)size;
    (void)flags;
    return nullptr;
}

void Manager::unmap(void* addr, size_t size)
{
    (void)addr;
    (void)size;
}

void Manager::mapPhysicalToVirtual(uintptr_t paddr, uintptr_t vaddr, enum MapFlags flags)
{
    Arch::Memory::Address pAddress(paddr);
    Arch::Memory::Address vAddress(vaddr);

    // Assume page directory is mapped in
    struct Arch::Memory::DirectoryEntry& dirEntry = m_directory.entries[vAddress.virtualAddress().dirIndex];
    // Table and table entry are not valid until we're sure they exist (see check below)
    struct Arch::Memory::Table& table = getTable(vAddress.virtualAddress().dirIndex);
    struct Arch::Memory::TableEntry& tableEntry = table.entries[vAddress.virtualAddress().tableIndex];

    if (!dirEntry.present) {
        dirEntry = {
            .present = 1,
            .readWrite = 1,
            .usermode = 0,
            .writeThrough = 0,
            .cacheDisable = 0,
            .accessed = 0,
            .ignoredA = 0,
            .size = 0,
            .ignoredB = 0,
            // TODO: Use an address class here?
            .tableAddr = Physical::Manager::getPage() >> ARCH_PAGE_TABLE_ENTRY_SHIFT
        };

        memset(&table, 0, sizeof(table));
    }

    if (tableEntry.present) {
        panic("Attempted to map address that is already in use!");
    }

    tableEntry = {
        .present = 1,
        .readWrite = 1,
        .usermode = 0,
        .writeThrough = 0,
        .cacheDisable = 0,
        .accessed = 0,
        .dirty = 0,
        .pageAttrTable = 0,
        .global = 0,
        .unused = 0,
        .pageAddr = pAddress.page().pageAddr,
    };

    if (flags & READ_ONLY) {
        tableEntry.readWrite = 0;
    }
    if (flags & USERMODE) {
        tableEntry.usermode = 1;
    }
    if (flags & WRITE_THROUGH) {
        tableEntry.writeThrough = 1;
    }
    if (flags & CACHE_DISABLE) {
        tableEntry.cacheDisable = 1;
    }
}

struct Arch::Memory::Table& Manager::getTable(size_t directoryIndex)
{
    // 0xFFC00000 represents the end of the virtual address space
    // minus the size of ARCH_TABLE_ENTRIES (1024) * ARCH_TABLE_SIZE
    uint32_t tablesBase = 0xFFC00000;
    uint32_t tableAddr = tablesBase + directoryIndex * ARCH_PAGE_SIZE;
    return *((Arch::Memory::Table*)tableAddr);
}

uintptr_t Manager::findFirstFreePage()
{
    // FIXME: Handle edge cases where the start and end values are not aligned to the start of a page table

    Arch::Memory::Address startAddr(m_searchStart);
    Arch::Memory::Address endAddr(m_rangeStart + m_rangeSize);
    for (size_t dirIdx = startAddr.virtualAddress().dirIndex; dirIdx < endAddr.virtualAddress().dirIndex; dirIdx++)
    {
        struct Arch::Memory::DirectoryEntry& dirEntry = m_directory.entries[dirIdx];
        if (!dirEntry.present) {
            // Implicit conversion to uintptr_t
            return Arch::Memory::Address(Arch::Memory::VirtualAddress {
                .offset = 0,
                .tableIndex = 0,
                .dirIndex = dirIdx,
            });
        }

        // Always start at 0, unless this is the first iteration inside the directory entries, in which case
        // we want to start on the page table index correlating to the startAddr.
        size_t tableIdxStart = 0;
        if (dirIdx == startAddr.virtualAddress().dirIndex) {
            tableIdxStart = startAddr.virtualAddress().tableIndex;
        }

        Arch::Memory::Table& table = getTable(dirIdx);
        for (size_t tableIdx = tableIdxStart; tableIdx < endAddr.virtualAddress().tableIndex; tableIdx++)
        {
            struct Arch::Memory::TableEntry& tableEntry = table.entries[tableIdx];
            if (!tableEntry.present)
            {
                // Implicit conversion to uintptr_t
                return Arch::Memory::Address(Arch::Memory::VirtualAddress {
                    .offset = 0,
                    .tableIndex = tableIdx,
                    .dirIndex = dirIdx,
                });
            }

            // Loop into next table entry. Nothing free here.
        }

        // Loop into next directry entry. Nothing free here.
    }

    // Absolutely nothing free in this directory.
    return npos;
}

} // !namespace Memory::Virtual
