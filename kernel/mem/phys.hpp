#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    PX_MMAP_AVAILABLE,
    PX_MMAP_RESERVED,
    PX_MMAP_ACPI_NV,
    PX_MMAP_ACPI_RECLAIMABLE
} px_mmap_type_t;

typedef struct {
    uintptr_t base;
    size_t length;
    px_mmap_type_t type;
} px_mmap_entry_t;

typedef bool (*px_mmap_iter_t)(px_mmap_entry_t *, void *ctx);

void px_mem_phys_init(size_t mem_size, uintptr_t kernel_end, px_mmap_iter_t mmap_iter, void *mmap_iter_ctx);

size_t px_mem_phys_get_pagesize(void);

size_t px_mem_phys_find_free(size_t count);

void px_mem_phys_free(size_t page, size_t count);

bool px_mem_phys_is_available(size_t page, size_t count);

void px_mem_phys_use(size_t page, size_t count);
