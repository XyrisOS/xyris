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
    READ_ONLY,
    USERMODE,
    WRITE_THROUGH,
    CACHE_DISABLE,
};

class Manager {
public:
    Manager(struct Arch::Memory::Directory& dir, uintptr_t rangeStart, size_t rangeSize)
        : m_directory(dir)
        , m_rangeStart(rangeStart)
        , m_rangeSize(rangeSize)
        , m_searchStart(rangeStart)
    {
        // Default constructor
    }

    Manager(const char* lockName, struct Arch::Memory::Directory& dir, uintptr_t rangeStart, size_t rangeSize)
        : m_lock(lockName)
        , m_directory(dir)
        , m_rangeStart(rangeStart)
        , m_rangeSize(rangeSize)
        , m_searchStart(rangeStart)
    {
        // Named lock constructor

    }

    // TODO: Not virtual
    virtual void* map(size_t size, enum MapFlags flags);
    virtual void unmap(void* addr, size_t size);

private:
    Mutex m_lock;
    struct Arch::Memory::Directory& m_directory;
    size_t m_rangeStart;
    size_t m_rangeSize;
    size_t m_searchStart;

    static const size_t npos = SIZE_MAX;

    void mapPhysicalToVirtual(uintptr_t paddr, uintptr_t vaddr, enum MapFlags flags);
    struct Arch::Memory::Table& getTable(size_t directoryIndex);
    uintptr_t findFirstFreePage();
};

}
