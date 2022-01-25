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

static size_t pageCount = 16;     // Number of pages to request per chunk.
static size_t totalAllocated = 0; // Total bytes allocated
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

static inline void align(void* ptr)
{
    if (ALIGNMENT > 1) {
        ptr = (void*)((uintptr_t)ptr + ALIGN_INFO);

        uintptr_t diff = (uintptr_t)ptr & (ALIGNMENT - 1);
        if (diff) {
            diff = ALIGNMENT - diff;
            ptr = (void*)((uintptr_t)ptr + diff);
        }

        *((ALIGN_TYPE*)((uintptr_t)ptr - ALIGN_INFO)) = diff + ALIGN_INFO;
    }
}

void* malloc(size_t requestedSize)
{
    // Minor* minor;
    // Minor* newMinor;
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

    if (memoryList.Head() == nullptr) {
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
            memoryList.InsertAfter(major, nextBlock);
            major = nextBlock;
        }

        // Case 2: New block
        if (major->getFirst() == nullptr) {
            // FIXME: This seems like some voodoo we don't want...
            void* buffer = (void*)((uintptr_t)major + sizeof(Minor));
            Minor* minor = new (buffer) Minor(magicHeapOk, major, size, requestedSize);
            // TODO: Have to (void)minor since everything is taken care of in the constructor
            // and we don't need to access it.
            (void)minor;

            major->setUsage(major->getUsage() + size + sizeof(Minor));
            totalInUse += size;

            // FIXME: Also seems like some voodoo...
            ptr = (void*)((uintptr_t)major->getFirst() + sizeof(Minor));
            // FIXME: Can't call align() without getting a compiler string overflow warning/error
            //align(ptr);

            goto exit;
        }

    }

exit:
    heapLock.unlock();
    return ptr;
}
