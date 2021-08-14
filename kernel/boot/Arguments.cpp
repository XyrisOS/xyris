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
#include <dev/serial/rs232.hpp>
#include <lib/string.hpp>
#include <lib/assert.hpp>

namespace Boot {

static size_t _argPos = 0;
static Argument _args[MAX_ARGUMENTS];

Argument::Argument()
    : _callback(NULL)
{
    // Default constructor
}

/* Getters */
const char* Argument::getCommand() { return _arg; }
cmdline_cb_t Argument::getCallback() { return _callback; }
/* Setters */
void Argument::setCallback(cmdline_cb_t cb) { _callback = cb; }
void Argument::setArgument(const char* arg) { strncpy(_arg, arg, MAX_ARGUMENT_LEN); }

void parseCommandLine(char* cmdline)
{
    rs232::printf("Parsing cmdline...\n");
    for (size_t pos = 0; pos < _argPos; pos++) {
        Argument arg = _args[pos];
        const char* cmd = arg.getCommand();
        rs232::printf("Position: %d, cmd: %s\n", pos, cmd);
        if (strstr(cmdline, cmd)) {
            rs232::printf("Match. Callback\n");
            arg.getCallback()(cmd);
        }
    }
}

void registerArgument(const char* arg, cmdline_cb_t cb)
{
    assert(_argPos < MAX_ARGUMENTS);
    Argument nextArg = _args[_argPos++];
    nextArg.setCallback(cb);
    nextArg.setArgument(arg);
}

}
