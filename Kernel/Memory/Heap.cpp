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
#include <Library/LinkedList.hpp>
#include <Library/rand.hpp>
#include <Locking/Mutex.hpp>
#include <Panic.hpp>

#define HEAP_MAGIC 0x0B1E55ED
#define HEAP_DEATH 0xBADA110C

#define ALIGNMENT 16 // Memory byte alignment
#define ALIGN_TYPE uint8_t
#define ALIGN_INFO sizeof(ALIGN_TYPE) * 16 // Size of alignment info

class Minor;

class Major : public LinkedList::Node {
public:
    Major(size_t pages)
        : Node()
        , m_Pages(pages)
        , m_Size(pages * ARCH_PAGE_SIZE)
        , m_Usage(sizeof(Major))
        , m_First(nullptr)
    {
        // Default constructor
    }

    void setPages(size_t pages) { m_Pages = pages; }
    void setSize(size_t size) { m_Size = size; }
    void setUsage(size_t usage) { m_Usage = usage; }
    void setFirst(Minor* first) { m_First = first; }

    size_t getPages() { return m_Pages; }
    size_t getSize() { return m_Size; }
    size_t getUsage() { return m_Usage; }
    Minor* getFirst() { return m_First; }

private:
    size_t m_Pages;
    size_t m_Size;
    size_t m_Usage;
    Minor* m_First;
};

class Minor : public LinkedList::Node {
public:
    Minor(size_t magic, Major* major, size_t size, size_t requestedSize)
        : Node()
        , m_Block(major)
        , m_Magic(magic)
        , m_Size(size)
        , m_RequestedSize(requestedSize)
    {
        // Default constructor
    }

    Major* m_Block;
    size_t m_Magic;
    size_t m_Size;
    size_t m_RequestedSize;
};

static Mutex heapLock("heap");

static bool isMagicChosen = false;
static size_t magicHeapOk = HEAP_MAGIC;
static size_t magicHeapDead = HEAP_DEATH;

static LinkedList::LinkedList memoryList;
static Major* bestBet = nullptr; // Major block with most free memory

static const size_t pageCount = 16; // Number of pages to request per chunk.
static size_t totalAllocated = 0;   // Total bytes allocated
static size_t totalInUse = 0;       // Total bytes in use

namespace Memory::Heap {

void initialize()
{
    if (!isMagicChosen) {
        heapLock.lock();

        magicHeapOk = (size_t)rand();
        magicHeapDead = (size_t)rand();
        isMagicChosen = true;

        heapLock.unlock();
    }
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
        panic("Out of memory!");
    }

    Major* major = new (buffer) Major(pages);
    totalAllocated += major->getSize();

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

void* malloc(size_t requestedSize)
{
    size_t size = requestedSize;

    // Adjust size so that there's enough space to store alignment info and
    // align the memory to the correct position.
    if (ALIGNMENT > 1) {
        size += ALIGNMENT + ALIGN_INFO;
    }

    heapLock.lock();

    if (size == 0) {
        // TODO: Print warning
        return NULL;
    }

    if (memoryList.IsEmpty()) {
        // Initialization of first major block
        Major* root = allocateNewPage(size);
        if (root == nullptr) {
            panic("Failed to initialize heap root block");
        }

        memoryList.InsertFront(root);
    }

    int32_t startedBet = 0;
    uint64_t bestSize = 0;
    Major* major = reinterpret_cast<Major*>(memoryList.Head());

    if (bestBet) {
        bestSize = bestBet->getSize() - bestBet->getUsage();
        if (bestSize > (size + sizeof(Minor))) {
            major = bestBet;
            startedBet = 1;
        }
    }

    uintptr_t diff = 0;
    void* ptr = nullptr;
    while (major) {
        diff = major->getSize() - major->getUsage(); // Total block free memory
        if (bestSize < diff) {
            // Block has more free memory than previous best
            bestBet = major;
            bestSize = diff;
        }

        // Case 1: Not enough space in this major block
        if (diff < size + sizeof(Minor)) {
            if (major->Next()) {
                major = reinterpret_cast<Major*>(major->Next());
                continue;
            }

            if (startedBet == 0) {
                major = reinterpret_cast<Major*>(memoryList.Head());
                startedBet = 0;
                continue;
            }

            // Create a new major block and add it next to the current
            Major* nextBlock = allocateNewPage(size);
            if (nextBlock == nullptr) {
                panic("Failed to allocate new major block");
            }

            memoryList.InsertAfter(major, nextBlock);
            major = nextBlock;
        }

        // Case 2: New block
        if (major->getFirst() == nullptr) {
            // Get a pointer to the region of memory directly after the major block
            void* buffer = (void*)((uintptr_t)major + sizeof(Major));
            // Use this region of memory as a minor block header
            Minor* minor = new (buffer) Minor(magicHeapOk, major, size, requestedSize);

            major->setFirst(minor);
            major->setUsage(major->getUsage() + size + sizeof(Minor));
            totalInUse += size;

            // Update the pointer to the memory directly after the minor block
            ptr = (void*)((uintptr_t)major->getFirst() + sizeof(Minor));

            // Align the pointer to the nearest bounary (block headers may cause unalignment)
            align(&ptr);
            goto exit;
        }

        // Case 3: Block is in use and there's enough space at the start of the block
        diff = (uintptr_t)major->getFirst() - ((uintptr_t)major + sizeof(Major));

        // Space in the front?
        if (diff >= (size + sizeof(Minor))) {
            Minor* minor = major->getFirst();
            // Get a pointer to the region of memory directly after the major block
            void* buffer = (void*)((uintptr_t)major + sizeof(Major));
            // Use this region of memory as a minor block header
            Minor* previous = new (buffer) Minor(magicHeapOk, major, size, requestedSize);

            // TODO: Keep track of this using the linked list library somehow?
            minor->SetPrevious(previous);
            previous->SetNext(minor);
            major->setUsage(major->getUsage() + size + sizeof(Minor));
            totalInUse += size;

            // Update the pointer to the memory directly after the minor block
            ptr = (void*)((uintptr_t)major->getFirst() + sizeof(Minor));

            // Align the pointer to the nearest bounary (block headers may cause unalignment)
            align(&ptr);
            goto exit;
        }

        // Case 4: There is enough space in this block, but is it contiguous?
        // Minor* newMinor;
        Minor* minor = major->getFirst();
        // Loop within the block and check contiguity
        while (minor) {
            // Case 4.1: End of minor block in major block.
            if (minor->Next()) {
                // The rest of the block is free, but is it big enough?
                diff = ((uintptr_t)major + major->getSize()) - ((uintptr_t)minor + sizeof(Minor) + minor->m_Size);
                if (diff >= (size + sizeof(Minor))) {
                    // Enough contiguous memory
                    void* buffer = (void*)((uintptr_t)minor + sizeof(Minor) + minor->m_Size);
                    // Use this region of memory as a minor block header
                    Minor* next = new (buffer) Minor(magicHeapOk, major, size, requestedSize);
                    next->SetPrevious(minor);
                    minor->SetNext(next);
                    major->setUsage(major->getUsage() + size + sizeof(Minor));
                    totalInUse += size;

                    // Update the pointer to the memory directly after the minor block
                    ptr = (void*)((uintptr_t)minor + sizeof(Minor));

                    // Align the pointer to the nearest bounary (block headers may cause unalignment)
                    align(&ptr);
                    goto exit;
                }
            }

            // Case 4.2: Is there space between two minor blocks?
            if (minor->Next()) {
                // Is the difference between this minor block and the next enough?
                diff = (uintptr_t)minor->Next() - ((uintptr_t)minor + sizeof(Minor) + minor->m_Size);
                if (diff >= (size + sizeof(Minor))) {
                    // Enough contiguous memory
                    void* buffer = (void*)((uintptr_t)minor + sizeof(Minor) + minor->m_Size);
                    // Use this region of memory as a minor block header
                    Minor* newMinor = new (buffer) Minor(magicHeapOk, major, size, requestedSize);
                    // TODO: Keep track of this using the linked list library somehow?
                    newMinor->SetNext(minor->Next());
                    newMinor->SetPrevious(minor);
                    minor->Next()->SetPrevious(newMinor);
                    minor->SetNext(newMinor);
                    major->setUsage(major->getUsage() + size + sizeof(Minor));
                    totalInUse += size;

                    // Update the pointer to the memory directly after the minor block
                    ptr = (void*)((uintptr_t)newMinor + sizeof(Minor));

                    // Align the pointer to the nearest bounary (block headers may cause unalignment)
                    align(&ptr);
                    goto exit;
                }
            }

            minor = reinterpret_cast<Minor*>(minor->Next());
        }

        // Case 5: Block is full.
        if (major->Next()) {
            if (startedBet == 1) {
                major = reinterpret_cast<Major*>(memoryList.Head());
                startedBet = 0;
                continue;
            }

            // Run out of page space.
            Major* nextBlock = allocateNewPage(size);
            if (nextBlock == nullptr) {
                panic("Failed to allocate new major block");
            }

            memoryList.InsertAfter(major, nextBlock);
        }

        major = reinterpret_cast<Major*>(major->Next());
    }

exit:
    heapLock.unlock();
    return ptr;
}
