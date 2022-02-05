/**
 * @file Heap.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2022-01-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */
#include "Heap.hpp"
#include "New.hpp"
#include "paging.hpp"
#include <Arch/Memory.hpp>
#include <Devices/Clock/rtc.hpp>
#include <Library/LinkedList.hpp>
#include <Library/rand.hpp>
#include <Library/stdio.hpp>
#include <Library/string.hpp>
#include <Locking/Mutex.hpp>
#include <Locking/RAII.hpp>
#include <Logger.hpp>
#include <Panic.hpp>

#define HEAP_MAGIC 0x0B1E55ED
#define HEAP_DEATH 0xBADA110C

#define ALIGNMENT 16 // Memory byte alignment
#define ALIGN_TYPE uint8_t
#define ALIGN_INFO sizeof(ALIGN_TYPE) * 16 // Size of alignment info

class Minor;

class Major : public LinkedList::Node {
public:
    explicit Major(size_t pages)
        : Node()
        , m_pages(pages)
        , m_size(pages * ARCH_PAGE_SIZE)
        , m_usage(sizeof(Major))
    {
        // Default constructor
    }

    void setPages(size_t pages) { m_pages = pages; }
    void setSize(size_t size) { m_size = size; }
    void setUsage(size_t usage) { m_usage = usage; }

    size_t pages() { return m_pages; }
    size_t size() { return m_size; }
    size_t usage() { return m_usage; }

    LinkedList::LinkedListUnmanaged llMinor;

private:
    size_t m_pages;
    size_t m_size;
    size_t m_usage;
};

class Minor : public LinkedList::Node {
public:
    Minor(size_t magic, Major* major, size_t size, size_t requestedSize)
        : Node()
        , m_block(major)
        , m_magic(magic)
        , m_size(size)
        , m_requestedSize(requestedSize)
    {
        // Default constructor
    }

    void setBlock(Major* block) { m_block = block; }
    void setMagic(size_t magic) { m_magic = magic; }
    void setSize(size_t size) { m_size = size; }
    void setRequestedSize(size_t requestedSize) { m_requestedSize = requestedSize; }

    Major* block() { return m_block; }
    size_t magic() { return m_magic; }
    size_t size() { return m_size; }
    size_t requestedSize() { return m_requestedSize; }

private:
    Major* m_block;
    size_t m_magic;
    size_t m_size;
    size_t m_requestedSize;
};

static Mutex heapLock("heap");

static bool isMagicChosen = false;
static size_t magicHeapOk = HEAP_MAGIC;
static size_t magicHeapDead = HEAP_DEATH;

static LinkedList::LinkedListUnmanaged memoryList;
static Major* bestBet = nullptr; // Major block with most free memory

static const size_t pageCount = 16; // Number of pages to request per chunk.
static size_t totalAllocated = 0;   // Total bytes allocated
static size_t totalInUse = 0;       // Total bytes in use

namespace Memory::Heap {

void init()
{
    if (!isMagicChosen) {
        RAIIMutex raii(heapLock);

        srand(RTC::getEpoch());
        magicHeapOk = (size_t)rand();
        magicHeapDead = (size_t)rand();
        isMagicChosen = true;

        Logger::Debug(__func__, "heap: OK: 0x%08zX DEAD: 0x%08zX\n", magicHeapOk, magicHeapDead);
    }
}

size_t getTotalAllocated()
{
    return totalAllocated;
}

size_t getTotalInUse()
{
    return totalInUse;
}

} // !namespace Memory::Heap

static Major* allocateNewPage(size_t size)
{
    // Allocate enough to fit the data plus a major and minor block header
    size_t pages = size + sizeof(Major) + sizeof(Minor);

    // Align the buffer size if necessary
    if (pages % ARCH_PAGE_SIZE == 0) {
        pages = pages / ARCH_PAGE_SIZE;
    } else {
        pages = pages / ARCH_PAGE_SIZE + 1;
    }

    // Ensure pages is at least equal to the minumum size;
    if (pages < pageCount) {
        pages = pageCount;
    }

    void* buffer = Memory::newPage(pages * ARCH_PAGE_SIZE - 1);
    if (buffer == nullptr) {
        panic("heap: out of memory!");
    }

    Major* major = new (buffer) Major(pages);
    totalAllocated += major->size();

    return major;
}

static inline void align(void** ptr)
{
    if (ALIGNMENT > 1) {
        *ptr = (void*)((uintptr_t)*ptr + ALIGN_INFO);

        uintptr_t diff = (uintptr_t)*ptr & (ALIGNMENT - 1);
        if (diff) {
            diff = ALIGNMENT - diff;
            *ptr = (void*)((uintptr_t)*ptr + diff);
        }

        *((ALIGN_TYPE*)((uintptr_t)*ptr - ALIGN_INFO)) = diff + ALIGN_INFO;
    }
}

static inline void unalign(void** ptr)
{
    if (ALIGNMENT > 1) {
        uintptr_t diff = *((ALIGN_TYPE*)((uintptr_t)*ptr - ALIGN_INFO));
        if (diff < (ALIGNMENT + ALIGN_INFO)) {
            *ptr = (void*)((uintptr_t)*ptr - diff);
        }
    }
}

void* malloc(size_t requestedSize)
{
    RAIIMutex raii(heapLock);
    Logger::Debug(__func__, "heap: mallocing: %zu\n", requestedSize);
    size_t size = requestedSize;

    // Adjust size so that there's enough space to store alignment info and
    // align the memory to the correct position.
    if (ALIGNMENT > 1) {
        size += ALIGNMENT + ALIGN_INFO;
    }

    if (size == 0) {
        Logger::Debug(__func__, "heap: malloc returning nullptr\n");
        return nullptr;
    }

    if (memoryList.IsEmpty()) {
        // Initialization of first major block
        Major* root = allocateNewPage(size);
        if (root == nullptr) {
            panic("heap: root block allocation failed");
        }

        memoryList.InsertFront(root);
    }

    int32_t startedBet = 0;
    uint64_t bestSize = 0;
    Major* major = static_cast<Major*>(memoryList.Head());

    if (bestBet) {
        bestSize = bestBet->size() - bestBet->usage();
        if (bestSize > (size + sizeof(Minor))) {
            major = bestBet;
            startedBet = 1;
        }
    }

    void* ptr = nullptr;
    while (major) {
        uintptr_t diff = major->size() - major->usage(); // Total block free memory
        if (bestSize < diff) {
            // Block has more free memory than previous best
            bestBet = major;
            bestSize = diff;
        }

        // Case 1: Not enough space in this major block
        if (diff < size + sizeof(Minor)) {
            if (major->Next()) {
                major = static_cast<Major*>(major->Next());
                continue;
            }

            if (startedBet == 0) {
                major = static_cast<Major*>(memoryList.Head());
                startedBet = 0;
                continue;
            }

            // Create a new major block and add it next to the current
            Major* nextBlock = allocateNewPage(size);
            if (nextBlock == nullptr) {
                panic("heap: major block allocation failed");
            }

            memoryList.InsertAfter(major, nextBlock);
            major = nextBlock;
        }

        // Case 2: New block
        if (major->llMinor.Head() == nullptr) {
            // Get a pointer to the region of memory directly after the major block
            void* buffer = (void*)((uintptr_t)major + sizeof(Major));
            // Use this region of memory as a minor block header
            Minor* minor = new (buffer) Minor(magicHeapOk, major, size, requestedSize);
            major->llMinor.InsertFront(minor);
            major->setUsage(major->usage() + size + sizeof(Minor));
            totalInUse += size;

            // Update the pointer to the memory directly after the minor block
            ptr = (void*)((uintptr_t)major->llMinor.Head() + sizeof(Minor));

            // Align the pointer to the nearest bounary (block headers may cause unalignment)
            align(&ptr);
            Logger::Debug(__func__, "heap: malloc'd: %p @ 2\n", ptr);
            return ptr;
        }

        // Case 3: Block is in use and there's enough space at the start of the block
        diff = (uintptr_t)major->llMinor.Head() - ((uintptr_t)major + sizeof(Major));

        // Space in the front?
        if (diff >= (size + sizeof(Minor))) {
            Minor* minor = static_cast<Minor*>(major->llMinor.Head());
            // Get a pointer to the region of memory directly after the major block
            void* buffer = (void*)((uintptr_t)major + sizeof(Major));
            // Use this region of memory as a minor block header
            Minor* newMinor = new (buffer) Minor(magicHeapOk, major, size, requestedSize);
            major->llMinor.InsertBefore(newMinor, minor);
            major->setUsage(major->usage() + size + sizeof(Minor));
            totalInUse += size;

            // Update the pointer to the memory directly after the minor block
            ptr = (void*)((uintptr_t)major->llMinor.Head() + sizeof(Minor));

            // Align the pointer to the nearest bounary (block headers may cause unalignment)
            align(&ptr);
            Logger::Debug(__func__, "heap: malloc'd: %p @ 3\n", ptr);
            return ptr;
        }

        // Case 4: There is enough space in this block, but is it contiguous?
        Minor* minor = static_cast<Minor*>(major->llMinor.Head());
        // Loop within the block and check contiguity
        while (minor) {
            // Case 4.1: End of minor block in major block.
            if (minor->Next() == nullptr) {
                // The rest of the block is free, but is it big enough?
                diff = ((uintptr_t)major + major->size()) - ((uintptr_t)minor + sizeof(Minor) + minor->size());
                if (diff >= (size + sizeof(Minor))) {
                    // Enough contiguous memory
                    void* buffer = (void*)((uintptr_t)minor + sizeof(Minor) + minor->size());
                    // Use this region of memory as a minor block header
                    Minor* next = new (buffer) Minor(magicHeapOk, major, size, requestedSize);
                    next->SetPrevious(minor);
                    minor->SetNext(next);
                    major->setUsage(major->usage() + size + sizeof(Minor));
                    totalInUse += size;

                    // Update the pointer to the memory directly after the minor block
                    ptr = (void*)((uintptr_t)minor + sizeof(Minor));

                    // Align the pointer to the nearest bounary (block headers may cause unalignment)
                    align(&ptr);
                    Logger::Debug(__func__, "heap: malloc'd: %p @ 4.1\n", ptr);
                    return ptr;
                }
            }

            // Case 4.2: Is there space between two minor blocks?
            if (minor->Next()) {
                // Is the difference between this minor block and the next enough?
                diff = (uintptr_t)minor->Next() - ((uintptr_t)minor + sizeof(Minor) + minor->size());
                if (diff >= (size + sizeof(Minor))) {
                    // Enough contiguous memory
                    void* buffer = (void*)((uintptr_t)minor + sizeof(Minor) + minor->size());
                    // Use this region of memory as a minor block header
                    Minor* newMinor = new (buffer) Minor(magicHeapOk, major, size, requestedSize);
                    major->llMinor.InsertBefore(newMinor, minor);
                    major->setUsage(major->usage() + size + sizeof(Minor));
                    totalInUse += size;

                    // Update the pointer to the memory directly after the minor block
                    ptr = (void*)((uintptr_t)newMinor + sizeof(Minor));

                    // Align the pointer to the nearest bounary (block headers may cause unalignment)
                    align(&ptr);
                    Logger::Debug(__func__, "heap: malloc'd: %p @ 4.2\n", ptr);
                    return ptr;
                }
            }

            minor = static_cast<Minor*>(minor->Next());
        }

        // Case 5: Block is full.
        if (major->Next()) {
            if (startedBet == 1) {
                major = static_cast<Major*>(memoryList.Head());
                startedBet = 0;
                continue;
            }

            // Run out of page space.
            Major* nextBlock = allocateNewPage(size);
            if (nextBlock == nullptr) {
                panic("heap: major block allocation failed");
            }

            memoryList.InsertAfter(major, nextBlock);
        }

        major = static_cast<Major*>(major->Next());
    }

    if (ptr) {
        Logger::Debug(__func__, "heap: malloc'd: %p @ end\n", ptr);
    } else {
        Logger::Debug(__func__, "heap: malloc returning nullptr @ end\n");
    }
    return ptr;
}

void free(void* ptr)
{
    RAIIMutex raii(heapLock);
    Logger::Debug(__func__, "heap: freeing: %p\n", ptr);
    if (ptr == nullptr) {
        return;
    }

    Minor* minor = static_cast<Minor*>((Minor*)((uintptr_t)ptr - sizeof(Minor)));
    size_t magic = minor->magic();
    if (magic != magicHeapOk)
    {
        // Check for over-run errors
        if (((magic & 0xFFFFFF) == (magicHeapOk & 0xFFFFFF)) ||
            ((magic & 0xFFFF) == (magicHeapOk & 0xFFFF)) ||
            ((magic & 0xFF) == (magicHeapOk & 0xFF))) {
            panicf("heap: 1-3 byte overrun for magic 0x%08zX != 0x%08zX", magic, magicHeapOk);
        }

        if (magic == magicHeapDead) {
            panicf("heap: double free on %p", ptr);
        } else {
            panicf("heap: bad free called on %p", ptr);
        }

        return;
    }

    Major* major = minor->block();
    totalInUse -= minor->size();
    major->setUsage(major->usage() - (minor->size() + sizeof(Minor)));
    minor->setMagic(magicHeapDead);
    // Remove the minor from the major's list
    major->llMinor.Remove(minor);

    // Clean up the major block
    if (major->llMinor.IsEmpty())
    {
        // Major block is completely unused
        memoryList.Remove(major);
        if (bestBet == major) {
            bestBet = nullptr;
        }

        totalAllocated -= major->size();
        Memory::freePage(major, major->pages() * ARCH_PAGE_SIZE - 1);
    } else if (bestBet) {
        // If the current major block that had space freed now has more
        // free space than the best bet, update.
        if ((major->size() - major->usage()) > (bestBet->size() - bestBet->usage())) {
            bestBet = major;
        }
    }
}

void* calloc(size_t count, size_t size)
{
    size_t actualSize = count * size;
    void* ptr = malloc(actualSize);
    memset(ptr, 0, actualSize);
    return ptr;
}

void* realloc(void* originalPtr, size_t size)
{
    if (size == 0) {
        free(originalPtr);
        return nullptr;
    }

    if (originalPtr == nullptr) {
        return malloc(size);
    }

    // Unalign the pointer (if required)
    void* ptr = originalPtr;
    unalign(&ptr);

    // Begin the rellocation
    RAIIMutex raii(heapLock);

    Minor* minor = (Minor*)((uintptr_t)ptr - sizeof(Minor));
    size_t magic = minor->magic();
    if (magic != magicHeapOk) {
        // Check for over-run errors
        if (((magic & 0xFFFFFF) == (magicHeapOk & 0xFFFFFF)) ||
            ((magic & 0xFFFF) == (magicHeapOk & 0xFFFF)) ||
            ((magic & 0xFF) == (magicHeapOk & 0xFF))) {
            panicf("heap: 1-3 byte overrun for magic 0x%08zX != 0x%08zX", magic, magicHeapOk);
        }

        if (magic == magicHeapDead) {
            panicf("heap: multiple realloc on %p", ptr);
        } else {
            panicf("heap: bad realloc called on %p", ptr);
        }

        return nullptr;
    }

    size_t realSize = minor->requestedSize();
    if (realSize >= size) {
        minor->setRequestedSize(size);
        return originalPtr;
    }

    // Reallocate to a block larger than what we currently have.
    ptr = malloc(size);
    memcpy(ptr, originalPtr, realSize);
    free(originalPtr);

    return ptr;
}

void* operator new(size_t size)
{
    return malloc(size);
}

void* operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void* p)
{
    free(p);
}

void operator delete[](void* p)
{
    free(p);
}

void operator delete(void* p, long unsigned int)
{
    free(p);
}

void operator delete[](void* p, long unsigned int)
{
    free(p);
}
