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

#define MAX_ARGUMENTS 32
#define MAX_ARGUMENT_LEN 32

namespace Boot {

typedef void (*cmdline_cb_t)(const char* arg);

/**
 * @brief Parse a command line for registered arguments
 *
 * @param cmdline Command line string to be parsed
 */
void parseCommandLine(char* cmdline);

/**
 * @brief Register an argument and callback with the argument parser
 *
 * @param arg Argument to be matched. Do not include the `--`.
 * @param cb Callback to be called if a match is found
 */
void registerArgument(const char* arg, cmdline_cb_t cb);

}
