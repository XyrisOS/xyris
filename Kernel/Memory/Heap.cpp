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
#include "paging.hpp"
#include <Arch/Memory.hpp>
#include <Library/rand.hpp>
#include <Library/LinkedList.hpp>
#include <Locking/Mutex.hpp>
#include <Panic.hpp>

#define HEAP_MAGIC  0x0B1E55ED
#define HEAP_DEATH  0xBADA110C

#define ALIGNMENT   16 // Memory byte alignment
#define ALIGN_TYPE  uint8_t
#define ALIGN_INFO  sizeof(ALIGN_TYPE) * 16 // Size of alignment info

class Minor;

class Major : public LinkedList::Node {
public:
    void initialize(size_t pages)
    {
        m_Prev = nullptr;
        m_Next = nullptr;
        m_Pages = pages;
        m_Size = pages * ARCH_PAGE_SIZE;
        m_Usage = sizeof(Major);
        m_First = nullptr;
    }

    void setPages(size_t pages) { m_Pages = pages; }
    void setSize(size_t size) { m_Size = size; }
    void setUsage(size_t usage) { m_Usage = usage; }

    size_t getPages() { return m_Pages; }
    size_t getSize() { return m_Size; }
    size_t getUsage() { return m_Usage; }

private:
    Major* m_Prev;
    Major* m_Next;
    size_t m_Pages;
    size_t m_Size;
    size_t m_Usage;
    Minor* m_First;
};

class Minor : public LinkedList::Node {
public:
    void initialize()
    {
        prev = nullptr;
        next = nullptr;
        block = nullptr;
        magic = 0;
        size = 0;
        requested_size = 0;
    }

    Minor* prev;
    Minor* next;
    Major* block;
    size_t magic;
    size_t size;
    size_t requested_size;
};

static Mutex heapLock("heap");

static bool isMagicChosen = false;
static size_t magicHeapOk = HEAP_MAGIC;
static size_t magicHeapDead = HEAP_DEATH;

static LinkedList::LinkedList memoryList;
static Major* bestBet = nullptr;    // Major block with most free memory

static size_t pageCount = 16;       // Number of pages to request per chunk.
static size_t totalAllocated = 0;   // Total bytes allocated
//static size_t totalInUse = 0;       // Total bytes in use

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

    Major* major = (Major*)Memory::newPage(pages * ARCH_PAGE_SIZE - 1);
    if (major == nullptr) {
        // Out of memory.
        // TODO: Should we panic or just return NULL?
        panic("Out of memory!");
    }

    major->initialize(pages);
    totalAllocated += major->getSize();

    return major;
}

void* malloc(size_t requestedSize)
{
    //void* ptr = nullptr;
    //Minor* minor;
    //Minor* newMinor;
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

    uintptr_t diff;
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
            }
        }
    }

    heapLock.unlock();

    (void)startedBet;
    return NULL;
}
