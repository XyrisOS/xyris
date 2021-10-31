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
    Section() = default;

    Section(const size_t base, const size_t size)
        : m_base(base)
        , m_size(size)
        , m_type(Unknown)
    {
    }

    Section(const size_t base, const size_t size, const Type type)
        : m_base(base)
        , m_size(size)
        , m_type(type)
    {
    }

    ALWAYS_INLINE bool initialized()
    {
        return m_base && m_size;
    }

    ALWAYS_INLINE size_t base()
    {
        return m_base;
    }

    ALWAYS_INLINE size_t size()
    {
        return m_size;
    }

    ALWAYS_INLINE size_t pages()
    {
        return m_size / ARCH_PAGE_SIZE;
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

    ALWAYS_INLINE size_t end()
    {
        return m_base + m_size - 1;
    }

    ALWAYS_INLINE bool empty()
    {
        return m_size == 0;
    }

    ALWAYS_INLINE bool contains(uintptr_t addr)
    {
        return end() >= addr && addr <= base();
    }

private:
    size_t m_base;
    size_t m_size;
    enum Type m_type;
};

} // !namespace Memory
