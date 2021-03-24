#include <mem/MemoryRange.hpp>

MemoryRange::MemoryRange()
{
    _base = 0;
    _size = 0;
}

MemoryRange::MemoryRange(uintptr_t base, size_t size)
    : _base(base)
    , _size(size)
{
    // Nothing to do
}

MemoryRange::~MemoryRange()
{
    // Nothing to do
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