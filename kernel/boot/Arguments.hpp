/**
 * @file Arguments.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2021-08-10
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once
#include <lib/LinkedList.hpp>

#define MAX_ARGUMENT_LEN 32
#define KERNEL_PARAM(name, arg, callback) \
    SECTION(".arguments") struct Boot::argument __##name##_arg = \
    { arg, callback }

namespace Boot {

typedef void (*cmdline_cb_t)(const char* arg);
struct argument {
    char arg[MAX_ARGUMENT_LEN];
    cmdline_cb_t callback;
};

/**
 * @brief Parse a command line for registered arguments
 *
 * @param cmdline Command line string to be parsed
 */
void parseCommandLine(char* cmdline);

} // !namespace Boot

/* Moved outside of sections.hpp since this is only desired
   if creating a kernel argument */
extern struct Boot::argument _ARGUMENTS_START[0];
#define ARGUMENTS_START ((uintptr_t)&_ARGUMENTS_START)
extern struct Boot::argument _ARGUMENTS_END[0];
#define ARGUMENTS_END ((uintptr_t)&_ARGUMENTS_END)
