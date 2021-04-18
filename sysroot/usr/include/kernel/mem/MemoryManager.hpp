#pragma once
#include <mem/MemoryRange.hpp>
#include <mem/paging.hpp>
#include <lib/bitmap.hpp>
#include <lib/mutex.hpp>
#include <stdint.h>
#include <stddef.h>

class MemoryManager {
public:
    MemoryManager(size_t page_count);
    ~MemoryManager();
    size_t FindIndex(uintptr_t addr);
    void AddRange(MemoryRange range);
    void* New(size_t size);
    void Free(void* page, size_t size);
    bool Present(size_t addr);
    void MapKernel(px_virtual_address_t vaddr, size_t paddr);
    size_t GetCurrentPageDirectory();
    void DumpSerial(void);

private:
    size_t machinePageCount;
    px_mutex_t mutexPaging;

    uint32_t pageDirectoryAddr;
    px_page_table_t* pageDirectoryVirt[PAGE_ENTRIES];

    /* both of these must be page aligned for anything to work right at all */
    static px_page_directory_entry_t pageDirectoryPhys[PAGE_ENTRIES] __attribute__ ((section (".page_tables,\"aw\", @nobits#")));
    static px_page_table_t           pageTables[PAGE_ENTRIES]       __attribute__ ((section (".page_tables,\"aw\", @nobits#")));

    MemoryRange* _ranges;
    size_t _ranges_count;

    void RegisterInterruptHandler();
    void InitPagingDirectory();
    void MapEarlyMemory();
    void MapKernelMemory();
    size_t FindNextFreeVirtualAddress(int seq);
    size_t FindNextFreePhysicalPage();
    void MapKernelPageTable(size_t pd_idx, px_page_table_t* table);
    void SetPageDirectory(size_t page_directory);
    void Enable();
    void Disable();
};
