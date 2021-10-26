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
    ALWAYS_INLINE bool Initialized()
    {
        return m_base && m_size;
    }
    ALWAYS_INLINE size_t Base()
    {
        return m_base;
    }
    ALWAYS_INLINE size_t Size()
    {
        return m_size;
    }
    ALWAYS_INLINE enum Type Type()
    {
        return m_type;
    }
    const char* TypeString()
    {
        switch (Type()) {
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
    ALWAYS_INLINE void SetType(enum Type type)
    {
        m_type = type;
    }
    ALWAYS_INLINE size_t End()
    {
        return m_base + m_size - 1;
    }
    ALWAYS_INLINE bool Empty()
    {
        return m_size == 0;
    }
    ALWAYS_INLINE bool Contains(uintptr_t addr)
    {
        return End() >= addr && addr <= Base();
    }

private:
    size_t m_base;
    size_t m_size;
    enum Type m_type;
};

} // !namespace Memory
