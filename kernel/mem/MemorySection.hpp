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
#include <arch/Memory.hpp>
#include <meta/compiler.hpp>
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
        , m_end(0)
        , m_type(Unknown)
    {
    }

    Section(const uintptr_t base, const uintptr_t end)
        : m_base(base)
        , m_end(end)
        , m_type(Unknown)
    {
    }

    Section(const uintptr_t base, const uintptr_t end, const Type type)
        : m_base(base)
        , m_end(end)
        , m_type(type)
    {
    }

    ALWAYS_INLINE bool initialized()
    {
        return m_base && size();
    }

    ALWAYS_INLINE uintptr_t base()
    {
        return m_base;
    }

    ALWAYS_INLINE uintptr_t end()
    {
        return m_end;
    }

    ALWAYS_INLINE uintptr_t size()
    {
        return m_end - m_base;
    }

    ALWAYS_INLINE uintptr_t pages()
    {
        return size() / ARCH_PAGE_SIZE;
    }

    ALWAYS_INLINE enum Type type()
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

    ALWAYS_INLINE void setType(enum Type type)
    {
        m_type = type;
    }

    ALWAYS_INLINE bool empty()
    {
        return size() == 0;
    }

    ALWAYS_INLINE bool contains(uintptr_t addr)
    {
        return end() >= addr && addr <= base();
    }

private:
    uintptr_t m_base;
    uintptr_t m_end;
    enum Type m_type;
};

} // !namespace Memory
