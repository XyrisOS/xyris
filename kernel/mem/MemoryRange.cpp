#include <mem/MemoryRange.hpp>
#include <mem/paging.hpp>

MemoryRange MemoryRange::AlignUp(uintptr_t base, size_t size, MemoryRangeType type)
{
    MemoryRange range = MemoryRange();

    size_t padding = base % PAGE_SIZE;

    range._base = base - padding;
    range._size = PAGE_ALIGN_UP(size + padding);
    range._type = type;

    return range;
}

MemoryRange MemoryRange::AlignDown(uintptr_t base, size_t size, MemoryRangeType type)
{
    MemoryRange range = MemoryRange();

    size_t padding = ((base % PAGE_SIZE) ? (PAGE_SIZE - base % PAGE_SIZE) : 0);

    range._base = base + padding;
    range._size = PAGE_ALIGN_DOWN(size - padding);
    range._type = type;

    return range;
}

uintptr_t MemoryRange::Base()
{
    return _base;
}

size_t MemoryRange::Size()
{
    return _size;
}

uintptr_t MemoryRange::End()
{
    return (Base() + Size() - 1);
}

size_t MemoryRange::Count()
{
    return (Size() / PAGE_SIZE);
}

bool MemoryRange::Empty()
{
    return (Size() == 0);
}

bool MemoryRange::Aligned()
{
    return PAGE_ALIGNED(Base()) && PAGE_ALIGNED(Size());
}

bool MemoryRange::Contains(uintptr_t addr)
{
    return ((addr >= Base()) && (addr <= End()));
}

MemoryRangeType MemoryRange::Type()
{
    return _type;
}
