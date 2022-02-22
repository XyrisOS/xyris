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
#include <Locking/Mutex.hpp>

namespace Memory::Virtual {

class Manager {
public:
    Manager(struct Arch::Memory::Directory* dir)
        : m_directory(dir)
    {
        //
    }

    Manager(const char* lockName, struct Arch::Memory::Directory* dir)
        : m_lock(lockName)
        , m_directory(dir)
    {
        //
    }

    /**
     * @brief Allocate and map in new virtual memory.
     *
     * @param size Desired size in bytes. Will be mapped in ``ARCH_PAGE_SIZE`` chunks.
     * @return void* Pointer to allocated and mapped memory.
     */
    virtual void* map(size_t size);

    /**
     * @brief Free and unmap virtual memory.
     *
     * @param addr Pointer to memory region to be unmapped.
     * @param size Size in bytes of memory to be unmapped. Will be unmapped in ``ARCH_PAGE_SIZE` chunks.
     */
    virtual void unmap(void* addr, size_t size);

private:
    Mutex m_lock;
    struct Arch::Memory::Directory* m_directory;
};

}
