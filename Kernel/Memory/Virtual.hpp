/**
 * @file Virtual.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Virtual memory manager
 * @version 0.1
 * @date 2022-02-21
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#pragma once
#include <Arch/Memory.hpp>
#include <Memory/Physical.hpp>
#include <Locking/Mutex.hpp>

namespace Memory::Virtual {

enum MapFlags
{
    NONE = 0,
    READ_ONLY = 1,
    USERMODE = 2,
    WRITE_THROUGH = 4,
    CACHE_DISABLE = 8,
};

class Manager {
public:
    Manager(Arch::Memory::Directory& dir, uintptr_t rangeStart, size_t rangeSize)
        : m_directory(dir)
        , m_rangeStart(rangeStart)
        , m_rangeSize(rangeSize)
        , m_rangeEnd(rangeStart + rangeSize)
        , m_searchStart(rangeStart)
    {
        // Default constructor
    }

    Manager(const char* lockName, Arch::Memory::Directory& dir, uintptr_t rangeStart, size_t rangeSize)
        : m_lock(lockName)
        , m_directory(dir)
        , m_rangeStart(rangeStart)
        , m_rangeSize(rangeSize)
        , m_searchStart(rangeStart)
    {
        // Named lock constructor
    }

    void* map(uintptr_t addr, size_t size, enum MapFlags flags);
    void unmap(void* addr, size_t size);

    static const size_t npos = SIZE_MAX;

protected:
    Mutex m_lock;
    Arch::Memory::Directory& m_directory;
    size_t m_rangeStart;
    size_t m_rangeSize;
    size_t m_rangeEnd;
    size_t m_searchStart;

    void initDirectory();
    void mapPhysicalToVirtual(uintptr_t paddr, uintptr_t vaddr, enum MapFlags flags = NONE);
    Arch::Memory::Table& getTable(size_t directoryIndex);
    uintptr_t findFirstFreePageRange(size_t range);
    bool isPageFree(Arch::Memory::Address vaddr);
    bool isPageRangeFree(Arch::Memory::Address vaddr, size_t size);
    bool virtualToPhysical(Arch::Memory::Address vaddr, Arch::Memory::Address& result);
};

}
