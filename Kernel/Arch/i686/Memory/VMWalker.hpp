/**
 * @file VMWalker.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Virtual memory structure walker
 * @version 0.1
 * @date 2022-02-20
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once
#include <Arch/i686/Memory/Address.hpp>
#include <Arch/i686/Memory/Types.h>

namespace Arch::Memory {

class VMWalker {
public:
    VMWalker() = default;

    [[gnu::always_inline]] struct DirectoryEntry* getDirectoryEntry(struct Directory* dir, size_t idx)
    {
        return &dir->entries[idx];
    }

    [[gnu::always_inline]] struct DirectoryEntry* getDirectoryEntry(struct Directory* dir, struct Page* page)
    {
        return &dir->entries[page->dirIndex];
    }

    [[gnu::always_inline]] struct Table* getTable(struct DirectoryEntry* dirEntry)
    {
        return reinterpret_cast<struct Table*>(dirEntry->tableAddr);
    }

    [[gnu::always_inline]] struct TableEntry* getTableEntry(struct Table* table, size_t idx)
    {
        return &table->entries[idx];
    }

    [[gnu::always_inline]] struct Page* getPage(struct TableEntry* entry)
    {
        return reinterpret_cast<struct Page*>(entry->frameAddr);
    }

    [[gnu::always_inline]] struct Frame* getPageFrame(struct TableEntry* entry)
    {
        return reinterpret_cast<struct Frame*>(entry->frameAddr);
    }

    [[gnu::always_inline]] uintptr_t getVirtualAddress(struct TableEntry* entry)
    {
        return reinterpret_cast<uintptr_t>(entry->frameAddr);
    }

    [[gnu::always_inline]] Address getAddress(struct TableEntry* entry)
    {
        return Address(getVirtualAddress(entry));
    }
};

} // !namespace Arch::Memory
