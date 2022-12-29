/**
 * @file MemoryRange.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief Class used to categorize and range various sections of memory
 * @version 0.1
 * @date 2021-09-12
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <Arch/Memory.hpp>
#include <stddef.h>
#include <stdint.h>

namespace Memory {

enum Type {
    Available,
    Reserved,
    ACPI,
    NVS,
    Bad,
    Bootloader,
    Kernel,
    Unknown
};

class Section {
public:
    Section()
        : m_base(0)
        , m_size(0)
        , m_type(Unknown)
    {
    }

    Section(const uintptr_t base, const uintptr_t size)
        : m_base(base)
        , m_size(size)
        , m_type(Unknown)
    {
    }

    Section(const uintptr_t base, const uintptr_t size, const Type type)
        : m_base(base)
        , m_size(size)
        , m_type(type)
    {
    }

    [[gnu::always_inline]] bool initialized()
    {
        return m_base && size();
    }

    [[gnu::always_inline]] uintptr_t base()
    {
        return m_base;
    }

    [[gnu::always_inline]] uintptr_t end()
    {
        return m_base + m_size;
    }

    [[gnu::always_inline]] uintptr_t size()
    {
        return m_size;
    }

    [[gnu::always_inline]] uintptr_t pages()
    {
        return size() / ARCH_PAGE_SIZE;
    }

    [[gnu::always_inline]] enum Type type()
    {
        return m_type;
    }

    const char* typeString()
    {
        switch (type()) {
            case Available:
                return "Available";
            case Reserved:
                return "Reserved";
            case ACPI:
                return "ACPI";
            case NVS:
                return "NVS";
            case Bad:
                return "Bad";
            case Bootloader:
                return "Bootloader";
            case Kernel:
                return "Kernel";
            case Unknown:
                return "Unknown";
            default:
                return "Invalid!";
        }
    }

    [[gnu::always_inline]] void setType(enum Type type)
    {
        m_type = type;
    }

    [[gnu::always_inline]] bool empty()
    {
        return size() == 0;
    }

    [[gnu::always_inline]] bool contains(uintptr_t addr)
    {
        return end() >= addr && addr <= base();
    }

private:
    uintptr_t m_base;
    uintptr_t m_size;
    enum Type m_type;
};

} // !namespace Memory
