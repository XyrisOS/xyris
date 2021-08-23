/**
 * @file Arguments.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2021-08-10
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <boot/Arguments.hpp>
#include <lib/stdio.hpp>
#include <lib/string.hpp>
#include <meta/sections.hpp>

namespace Boot {

void parseCommandLine(char* cmdline)
{
    for (struct argument* arg = _ARGUMENTS_START; arg < _ARGUMENTS_END; arg++) {
        if (strstr(cmdline, arg->arg)) {
            arg->callback(arg->arg);
        }
    }
}

}
