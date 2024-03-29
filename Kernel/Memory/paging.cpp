/**
 * @file paging.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-22
 *
 * @copyright Copyright Keeton Feavel and Micah Switzer (c) 2019
 *
 */
#include <Arch/Memory.hpp>
#include <Library/Bitset.hpp>
#include <Memory/Physical.hpp>
#include <Memory/paging.hpp>
#include <Memory/Virtual.hpp>
#include <Support/sections.hpp>
#include <Panic.hpp>
#include <Logger.hpp>
#include <stddef.h>

namespace Memory {

static Mutex pagingLock("paging");

static Bitset<MEM_BITMAP_SIZE> virtualMemoryBitset;

// both of these must be page aligned for anything to work right at all
[[gnu::section(".page_tables,\"aw\", @nobits#")]] static struct Arch::Memory::Directory pageDirectory;
// page tables for the entire 32-bit address space
[[gnu::section(".page_tables,\"aw\", @nobits#")]] static struct Arch::Memory::Table pageTables[ARCH_PAGE_DIR_ENTRIES];

static void pageFaultCallback(struct registers* regs);
static void initDirectory();
static void mapEarlyMem();
static void mapKernel();
static uintptr_t findNextFreeVirtualAddress(size_t seq);
static void mapKernelPageTable(size_t idx, struct Arch::Memory::Table* table);
static Virtual::Manager virtualManager("virtual", pageDirectory, ARCH_DIR_ALIGN(KERNEL_START), ARCH_DIR_ALIGN_UP(KERNEL_END - KERNEL_START));

void init()
{
    // DONE: Move logic from this point until next TODO into Kernel.hpp/.cpp
    Interrupts::registerHandler(Interrupts::EXCEPTION_PAGE_FAULT, pageFaultCallback);
    initDirectory();
    // TODO: Move logic from this point on into Kernel.hpp/.cpp
    mapEarlyMem();  // Map early memory into kernel page tables in a 1:1 manner
    mapKernel();    // Map kernel into kernel page tables
    Arch::Memory::setPageDirectory(Arch::Memory::pageAlign(KADDR_TO_PHYS((uintptr_t)&pageDirectory)));
    Arch::Memory::pagingEnable();
}

static void pageFaultCallback(struct registers* regs)
{
    panic(regs);
}

static inline void mapKernelPageTable(size_t idx, struct Arch::Memory::Table* table)
{
    pageDirectory.entries[idx] = {
        .present = 1,
        .readWrite = 1,
        .usermode = 0,
        .writeThrough = 0,
        .cacheDisable = 0,
        .accessed = 0,
        .ignoredA = 0,
        .size = 0,
        .ignoredB = 0,
        // TODO: Get rid of this shift by using ``union Address``
        .tableAddr = (uint32_t)KADDR_TO_PHYS((uintptr_t)table) >> ARCH_PAGE_TABLE_ENTRY_SHIFT
    };
}

static void initDirectory()
{
    // For every page in kernel memory
    for (size_t i = 0; i < ARCH_PAGE_DIR_ENTRIES - 1; i++) {
        mapKernelPageTable(i, &pageTables[i]);
        // clear out the page tables
        for (size_t j = 0; j < ARCH_PAGE_TABLE_ENTRIES; j++) {
            memset(&pageTables[i].entries[j], 0, sizeof(struct Arch::Memory::TableEntry));
        }
    }
}

void mapKernelPage(Arch::Memory::Address vaddr, Arch::Memory::Address paddr)
{
    // Set the page directory entry (pde) and page table entry (pte)
    size_t pde = vaddr.virtualAddress().dirIndex;
    size_t pte = vaddr.virtualAddress().tableIndex;

    // Print a debug message to serial
    Logger::Trace(__func__, "map 0x%0zx to 0x%0zx, pde = 0x%0zx, pte = 0x%0zx", paddr.val(), vaddr.val(), pde, pte);

    // If the page's virtual address is not aligned
    if (vaddr.virtualAddress().offset) {
        panicf("Attempted to map a non-page-aligned virtual address.\n(Address: 0x%0zx)\n", vaddr.val());
    }

    // If the page is already mapped into memory
    Arch::Memory::TableEntry* entry = &(pageTables[pde].entries[pte]);
    if (entry->present) {
        if (entry->pageAddr == paddr.page().pageAddr) {
            // this page was already mapped the same way
            return;
        }

        panic("Attempted to map already mapped page.\n");
    }
    // Set the page information
    pageTables[pde].entries[pte] = {
        .present = 1,                       // The page is present
        .readWrite = 1,                     // The page has r/w permissions
        .usermode = 0,                      // These are kernel pages
        .writeThrough = 0,                  // Disable write through
        .cacheDisable = 0,                  // The page is cached
        .accessed = 0,                      // The page is unaccessed
        .dirty = 0,                         // The page is clean
        .pageAttrTable = 0,                 // The page has no attribute table
        .global = 0,                        // The page is local
        .unused = 0,                        // Ignored
        .pageAddr = paddr.page().pageAddr,  // Page physical address
    };
    // Set the associated bit in the bitmaps
    Physical::Manager::the().setUsed(paddr);
    virtualMemoryBitset.Set(vaddr.page().pageAddr);
}

void mapKernelRangeVirtual(Section sect)
{
    for (Arch::Memory::Address a(sect.base()); a < sect.end(); a += ARCH_PAGE_SIZE) {
        mapKernelPage(a, a);
    }
}

void mapKernelRangePhysical(Section sect)
{
    for (Arch::Memory::Address a(sect.base()); a < sect.end(); a += ARCH_PAGE_SIZE) {
        Arch::Memory::Address phys(KADDR_TO_PHYS(a));
        mapKernelPage(a, phys);
    }
}

static void mapEarlyMem()
{
    // identity map the first 1 MiB of RAM
    Logger::Debug(__func__, "==== MAP EARLY MEM ====");
    mapKernelRangeVirtual(Section(EARLY_MEM_START, EARLY_KERNEL_START - EARLY_MEM_START));
}

static void mapKernel()
{
    Logger::Debug(__func__, "==== MAP HH KERNEL ====");
    mapKernelRangePhysical(Section(Arch::Memory::pageAlign(KERNEL_START), Arch::Memory::pageAlignUp(KERNEL_SIZE)));
}

/**
 * note: this can't find more than 32 sequential pages
 * @param seq the number of sequential pages to get
 */
static uintptr_t findNextFreeVirtualAddress(size_t seq)
{
    return virtualMemoryBitset.FindFirstRange(seq, false);
}

void* newPage(size_t size)
{
    RAIIMutex lock(pagingLock);
    size_t page_count = PAGE_COUNT(size);
    size_t free_idx = findNextFreeVirtualAddress(page_count);

    if (free_idx == SIZE_MAX) {
        return NULL;
    }

    for (size_t i = free_idx; i < free_idx + page_count; i++) {
        size_t phys_page_idx = Physical::Manager::the().findNextFreePhysicalAddress();
        if (phys_page_idx == Physical::Manager::npos) {
            return NULL;
        }

        Arch::Memory::Address phys(phys_page_idx * ARCH_PAGE_SIZE);
        Arch::Memory::Address vaddr(i * ARCH_PAGE_SIZE);
        mapKernelPage(vaddr, phys);
    }

    return (void*)(free_idx * ARCH_PAGE_SIZE);
}

// TODO: Use assert here
void* newPageMustSucceed(size_t size)
{
    void* addr = newPage(size);
    if (addr == NULL) {
        panic("Failed to get new page!");
    }

    return addr;
}

void freePage(void* page, size_t size)
{
    RAIIMutex lock(pagingLock);
    size_t page_count = PAGE_COUNT(size);
    Arch::Memory::Address addr((uintptr_t)page);
    for (size_t i = addr.virtualAddress().tableIndex; i < addr.virtualAddress().tableIndex + page_count; i++) {
        virtualMemoryBitset.Clear(i);
        // this is the same as the line above
        struct Arch::Memory::TableEntry* pte = &(pageTables[i / ARCH_PAGE_TABLE_ENTRIES].entries[i % ARCH_PAGE_TABLE_ENTRIES]);
        // the frame field is actually the page frame's index basically it's frame 0, 1...(2^21-1)
        Physical::Manager::the().setFree(pte->pageAddr);
        // zero it out to unmap it
        memset(pte, 0, sizeof(struct Arch::Memory::TableEntry));
        // clear that tlb
        Arch::Memory::pageInvalidate(page);
    }
}

bool isPresent(uintptr_t addr)
{
    // Convert the address into an index and check whether the page is in the bitmap
    return virtualMemoryBitset[addr >> ARCH_PAGE_TABLE_ENTRY_SHIFT];
}

// TODO: maybe enforce access control here in the future
uintptr_t getPageDirPhysAddr()
{
    return KADDR_TO_PHYS((uintptr_t)&pageDirectory);
}

} // !namespace Paging
