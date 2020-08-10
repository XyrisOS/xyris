/**
 * @file trace.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-08-09
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */

#ifndef PANIX_SYS_TRACE_HPP
#define PANIX_SYS_TRACE_HPP

#include <stdint.h>
#include <arch/arch.hpp>

void px_stack_trace(unsigned int max);

#endif /* PANIX_SYS_TRACE_HPP */