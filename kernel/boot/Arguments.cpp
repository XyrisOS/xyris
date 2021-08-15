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
#include <lib/assert.hpp>

namespace Boot {

// Use a C style structure because we're early enough in the kernel initialization
// (since we're registering argument callbacks when constructing) that we can't
// use classes since there is no guarantee that we'll be constructed in order.
struct argument {
    char arg[MAX_ARGUMENT_LEN];
    cmdline_cb_t callback;
};

static size_t _argPos = 0;
static struct argument _args[MAX_ARGUMENTS];

void parseCommandLine(char* cmdline)
{
    debugf("Parsing cmdline...\n");
    for (size_t pos = 0; pos < _argPos; pos++) {
        struct argument* arg = &_args[pos];
        const char* str = arg->arg;
        if (strstr(cmdline, str)) {
            arg->callback(str);
        }
    }
}

void registerArgument(const char* arg, cmdline_cb_t cb)
{
    assert(_argPos < MAX_ARGUMENTS);
    struct argument* nextArg = &_args[_argPos++];
    nextArg->callback = cb;
    strncpy(nextArg->arg, arg, MAX_ARGUMENT_LEN);
}

}
