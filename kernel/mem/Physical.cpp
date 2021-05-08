#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <mem/Physical.hpp>
#include <lib/bitmap.hpp>
#include <lib/assert.hpp>
#include <lib/debugging.hpp>

namespace Memory {

// TODO: need a mutex to protect access
static bitmap_t *_phys_map = NULL;
// hard-coded for now, but can probably be gleaned from boot info
static size_t _page_count = 1024 * 1024;
static size_t _first_free = 0;

void PhysicalInit(size_t memSize, uintptr_t kernelEnd, px_mmap_iter_t mapIter, void *mapIterCtx)
{
    (void) memSize;
    (void) kernelEnd;
    (void) mapIter;
    (void) mapIterCtx;
    // TODO: allocate space for the physical memory bitmap
    // TODO: initialize the bitmap to nothing free
    // TODO: free regions indicated by the bootloader memory map
    // TODO: somehow don't actually free memory in use by the kernel?
    //       - or just re-map the kernel into the new address space as
    //         a part of the VM initialization
}

size_t PhysicalGetPageSize(void)
{
    // TODO: Make this architecture specific?
    return 4 * 1024;
}

size_t PhysicalFindFree(size_t count)
{
    return bitmap_find_first_range_clear(_phys_map, _first_free, _page_count, count);
}

void PhysicalFree(size_t page, size_t count)
{
    if (count == 0) {
        WARN("count == 0, doing nothing");
        return;
    }
    // TODO: optimize this
    for (size_t i = page; i < page + count; i++) {
        bitmap_clear_bit(_phys_map, i);
    }
    _first_free = page;
}

bool PhysicalIsAvailable(size_t page, size_t count)
{
    // TODO: optimize this
    for (size_t i = page; i < page + count; i++) {
        if (!bitmap_get_bit(_phys_map, i)) {
            return false;
        }
    }
    return true;
}

void PhysicalAlloc(size_t page, size_t count)
{
    if (page == _first_free) {
        _first_free = page + count;
    }
    // TODO: optimize this
    for (size_t i = page; i < page + count; i++) {
        bitmap_set_bit(_phys_map, i);
    }
}

}