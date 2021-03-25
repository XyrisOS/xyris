#if 0
#include <lib/string.hpp>
#include <dev/serial/rs232.hpp>

#include <mem/MemoryRange.hpp>

#define stringof(tok) #tok

size_t MemoryLayout::find_index(uintptr_t addr)
{
    size_t i;
    // the bad way (iterative) - it should be a binary search
    for (i = 0; i < count(); i++) {
        if (addr < _ranges[i].base()) break;
    }

    return i;
}

void MemoryLayout::add_range(MemoryRange range)
{
    size_t index = find_index(range.base());

    if (index != count()) {
        memmove(&_ranges[index + 1], &_ranges[index], (size() - index) * sizeof(*_ranges));
    }

    _ranges[index] = range;

    _count++;
}

void MemoryLayout::dump_serial(void)
{
    static const char *type_strings[] = {
        [MemoryUsable] = stringof(MemoryUsable),
        [MemoryReserved] = stringof(MemoryReserved)
    };
    for (size_t i = 0; i < count(); i++) {
        MemoryRange *range = _ranges + i;
        px_rs232_printf("  Base = 0x%p, Size = 0x%p, Type = %s\n",
            range->base(), range->size(), type_strings[range->type()]);
    }
}
#endif
