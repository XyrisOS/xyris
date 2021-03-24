#include <mem/paging.hpp>
#include <stddef.h>
#include <stdint.h>

class MemoryRange {
public:
        MemoryRange();
        MemoryRange(uintptr_t base, size_t size);
        ~MemoryRange();

        uintptr_t Base();
        size_t Size();
        uintptr_t End();
        size_t Count();
        bool Empty();
        bool Aligned();
        bool Contains(uintptr_t addr);
private:
        uintptr_t _base;
        size_t _size;
};
