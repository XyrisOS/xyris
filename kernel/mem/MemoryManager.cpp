#include <mem/MemoryManager.hpp>
#include <mem/MemoryRange.hpp>
#include <lib/string.hpp>
#include <dev/serial/rs232.hpp>

#define stringof(tok) #tok

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