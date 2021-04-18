/**
 * @file stack.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Stack management functionality
 * @version 0.1
 * @date 2021-02-21
 * 
 * @copyright Copyright the Panix Contributors (c) 2021
 * 
 */

#pragma once
#include <stdint.h>

// Used as a magic number for stack smashing protection
#if UINT32_MAX == UINTPTR_MAX
    #define STACK_CHK_GUARD 0xDEADC0DE
#else
    #define STACK_CHK_GUARD 0xBADBADBADBADBAD1
#endif

extern "C" void __stack_chk_fail(void);
