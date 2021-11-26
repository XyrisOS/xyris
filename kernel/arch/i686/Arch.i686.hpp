/**
 * @file Arch.i686.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief i686 architecture implementation of Arch.hpp
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <arch/i686/regs.hpp>
#include <arch/i686/ports.hpp>
#include <arch/i686/isr.hpp>

namespace Arch {

struct stackframe {
    struct stackframe* ebp;
    size_t eip;
};

} // !namespace Arch
