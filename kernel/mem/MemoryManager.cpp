#include <mem/MemoryManager.hpp>
#include <mem/MemoryRange.hpp>
#include <lib/string.hpp>
#include <dev/serial/rs232.hpp>

#define stringof(tok) #tok

MemoryManager::MemoryManager(size_t page_count)
{
    (void)page_count;
}


MemoryManager::~MemoryManager()
{

}

size_t MemoryManager::FindIndex(uintptr_t addr)
{
    size_t i;
    // the bad way (iterative) - it should be a binary search
    for (i = 0; i < _ranges_count; i++) {
        if (addr < _ranges[i].Base()) break;
    }

    return i;
}

void MemoryManager::AddRange(MemoryRange range)
{
    size_t index = FindIndex(range.Base());

    if (index != _ranges_count) {
        memmove(&_ranges[index + 1], &_ranges[index], (range.Size() - index) * sizeof(*_ranges));
    }

    _ranges[index] = range;
    _ranges_count++;
}

void* MemoryManager::New(size_t size)
{
    (void)size;
    return NULL;
}

void MemoryManager::Free(void* page, size_t size)
{
    (void)page;
    (void)size;
}

bool MemoryManager::Present(size_t addr)
{
    (void)addr;
    return false;
}

void MemoryManager::MapKernel(px_virtual_address_t vaddr, size_t paddr)
{
    (void)vaddr;
    (void)paddr;
}

size_t MemoryManager::GetCurrentPageDirectory()
{
    return 0;
}

void MemoryManager::DumpSerial(void)
{
    static const char* type_strings[] = {
        [MemoryUsable] = stringof(MemoryUsable),
        [MemoryReserved] = stringof(MemoryReserved)
    };
    for (size_t i = 0; i < _ranges_count; i++) {
        MemoryRange *range = _ranges + i;
        px_rs232_printf("  Base = 0x%p, Size = 0x%p, Type = %s\n",
            range->Base(), range->Size(), type_strings[range->Type()]);
    }
}

void MemoryManager::RegisterInterruptHandler()
{

}

void MemoryManager::InitPagingDirectory()
{

}

void MemoryManager::MapEarlyMemory()
{

}

void MemoryManager::MapKernelMemory()
{

}

size_t MemoryManager::FindNextFreeVirtualAddress(int seq)
{
    (void)seq;
    return 0;
}

size_t MemoryManager::FindNextFreePhysicalPage()
{
    return 0;
}

void MemoryManager::MapKernelPageTable(size_t pd_idx, px_page_table_t* table)
{
    (void)pd_idx;
    (void)table;
}

void MemoryManager::SetPageDirectory(size_t page_directory)
{
    (void)page_directory;
}

void MemoryManager::Enable()
{

}

void MemoryManager::Disable()
{

}
