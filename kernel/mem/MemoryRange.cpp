#include <mem/MemoryRange.hpp>
#include <mem/paging.hpp>

namespace Memory {

MemoryRange MemoryRange::AlignUp(uintptr_t base, size_t size, MemoryType type)
{
    MemoryRange range = MemoryRange();

    size_t padding = base % PAGE_SIZE;

    range.base = base - padding;
    range.size = PAGE_ALIGN_UP(size + padding);
    range.type = type;

    return range;
}

MemoryRange MemoryRange::AlignDown(uintptr_t base, size_t size, MemoryType type)
{
    MemoryRange range = MemoryRange();

    size_t padding = ((base % PAGE_SIZE) ? (PAGE_SIZE - base % PAGE_SIZE) : 0);

    range.base = base + padding;
    range.size = PAGE_ALIGN_DOWN(size - padding);
    range.type = type;

    return range;
}

uintptr_t MemoryRange::Base()
{
    return base;
}

size_t MemoryRange::Size()
{
    return size;
}

MemoryType MemoryRange::Type()
{
    return type;
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

}
