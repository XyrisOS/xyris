#pragma once
#include <mem/MemoryRange.hpp>
#include <mem/paging.hpp>
#include <lib/bitmap.hpp>
#include <lib/mutex.hpp>
#include <stdint.h>
#include <stddef.h>

namespace Memory
{

void Initialize(void *bootinfo);

void Dump();

size_t GetUsed();

size_t GetTotal();

int Map(void *address_space, MemoryRange range, uint32_t flags);

int MapIdentity(void *address_space, MemoryRange range, uint32_t flags);

int Alloc(void *address_space, size_t size, uint32_t flags, uintptr_t *out_address);

int AllocIdentity(void *address_space, uint32_t flags, uintptr_t *out_address);

int Free(void *address_space, MemoryRange range);

}
