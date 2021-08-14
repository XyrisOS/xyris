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

namespace Boot {

size_t ArgumentParser::_argPos = 0;
Argument ArgumentParser::_args[MAX_ARGUMENTS];

Argument::Argument()
    : _callback(NULL)
{
    // Default constructor
}

/* Getters */
const char* Argument::getCommand() { return _cmd; }
cmdline_cb_t Argument::getCallback() { return _callback; }
/* Setters */
void Argument::setCallback(cmdline_cb_t cb) { _callback = cb; }
void Argument::setCommand(const char* cmd) { strncpy(_cmd, cmd, MAX_ARGUMENT_LEN); }

void ArgumentParser::parse(char* cmdline)
{
    rs232::printf("Parsing cmdline...\n");
    for (size_t pos = 0; pos < ArgumentParser::_argPos; pos++) {
        Argument arg = ArgumentParser::_args[pos];
        const char* cmd = arg.getCommand();
        rs232::printf("Position: %d, cmd: %s\n", pos, cmd);
        if (strstr(cmdline, cmd)) {
            rs232::printf("Match. Callback\n");
            arg.getCallback()(cmd);
        }
    }
}

bool ArgumentParser::registerArgument(const char* arg, cmdline_cb_t cb)
{
    if (_argPos == MAX_ARGUMENTS)
        return false;

    Argument nextArg = ArgumentParser::_args[ArgumentParser::_argPos++];
    nextArg.setCallback(cb);
    nextArg.setCommand(arg);

    return true;
}

}
