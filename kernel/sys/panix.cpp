/**
 * @file sys.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-14
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */

#include <sys/panix.hpp>

// List of all exceptions and their associated english descriptions
const char* px_exception_descriptions[] = {
    "Divide-By-Zero\0", "Debugging\0", "Non-Maskable\0", "Breakpoint\0",
    "Overflow\0", "Out Bound Range\0", "Invalid Opcode\0", "Device Not Avbl\0",
    "Double Fault\0", "Co-CPU Overrun\0", "Invalid TSS\0", "Sgmnt !Present\0",
    "Seg Fault\0", "Protection Flt\0", "Page Fault\0", "RESERVED\0",
    "Floating Pnt\0", "Alignment Check\0", "Machine Check\0", "SIMD Flt Pnt\0",
    "Virtualization\0", "RESERVED\0", "RESERVED\0", "RESERVED\0",
    "RESERVED\0", "RESERVED\0", "RESERVED\0", "RESERVED\0",
    "RESERVED\0", "Security Excptn\0", "RESERVED\0", "Triple Fault\0", "FPU Error\0"
};