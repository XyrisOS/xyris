#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <mem/phys.hpp>
#include <lib/bitmap.hpp>
#include <lib/assert.hpp>
#include <lib/debugging.hpp>

// TODO: need a mutex to protect access
static bitmap_t *_phys_map = NULL;
// hard-coded for now, but can probably be gleaned from boot info
static size_t _page_count = 1024 * 1024;
static size_t _first_free = 0;

void px_mem_phys_init(size_t mem_size, uintptr_t kernel_end, px_mmap_iter_t mmap_iter, void *mmap_iter_ctx)
{
    (void) mem_size;
    (void) kernel_end;
    (void) mmap_iter;
    (void) mmap_iter_ctx;
    // TODO: allocate space for the physical memory bitmap
    // TODO: initialize the bitmap to nothing free
    // TODO: free regions indicated by the bootloader memory map
    // TODO: somehow don't actually free memory in use by the kernel?
    //       - or just re-map the kernel into the new address space as
    //         a part of the VM initialization
}

size_t px_mem_phys_get_pagesize(void)
{
    return 4 * 1024;
}

size_t px_mem_phys_find_free(size_t count)
{
    return bitmap_find_first_range_clear(_phys_map, _first_free, _page_count, count);
}

void px_mem_phys_free(size_t page, size_t count)
{
    if (count == 0) {
        WARN("count == 0, doing nothing");
        return;
    }
    // TODO: optimize this
    for (size_t i = page; i < page + count; i++)
        bitmap_clear_bit(_phys_map, i);
    _first_free = page;
}

bool px_mem_phys_is_available(size_t page, size_t count)
{
    // TODO: optimize this
    for (size_t i = page; i < page + count; i++)
        if (!bitmap_get_bit(_phys_map, i)) return false;
    return true;
}

void px_mem_phys_use(size_t page, size_t count)
{
    if (page == _first_free)
        _first_free = page + count;
    // TODO: optimize this
    for (size_t i = page; i < page + count; i++)
        bitmap_set_bit(_phys_map, i);
}
