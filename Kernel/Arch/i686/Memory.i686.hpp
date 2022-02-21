/**
 * @file Memory.i686.hpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief i686 memory structures and definitions. C & C++ compatible header.
 * @version 0.1
 * @date 2021-10-26
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Arch::Memory {
// TODO: Maybe rename these headers? Can't think of anything much better...
#include <Arch/i686/Memory/Types.h>
#include <Arch/i686/Memory/Functions.h>

/**
 * @brief Address class. Represents a memory address that can
 * be used to address a page in virtual memory, a frame in
 * physical memory. Allows setting the value of these addresses
 * by also including an uintptr_t representation.
 *
 */
class Address
{
public:
    Address(uintptr_t addr) {
        m_addr.val = addr;
    }

    Address(struct Frame frame) {
        m_addr.frame = frame;
    }

    Address(struct Page page) {
        m_addr.page = page;
    }

    operator struct Page() {
        return m_addr.page;
    }

    operator struct Frame() {
        return m_addr.frame;
    }

    operator uintptr_t() {
        return m_addr.val;
    }

    uintptr_t operator+= (const uintptr_t& val) {
        m_addr.val += val;
        return m_addr.val;
    }

    uintptr_t operator-= (const uintptr_t& val) {
        m_addr.val -= val;
        return m_addr.val;
    }

    uintptr_t operator= (const uintptr_t& val) {
        m_addr.val = val;
        return m_addr.val;
    }

    struct Page page() {
        return m_addr.page;
    }

    struct Frame frame() {
        return m_addr.frame;
    }

    uintptr_t val() {
        return m_addr.val;
    }

private:
    union {
        struct Page page;
        struct Frame frame;
        uintptr_t val;
    } m_addr;
};

} // !namespace Arch::Memory
