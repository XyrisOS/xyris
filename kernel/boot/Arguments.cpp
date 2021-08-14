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

#define ARGUMENT_DELIMETER ' '

namespace Boot {

// Private Static Member Initialization
char* ArgumentParser::_cmdline = NULL;
size_t ArgumentParser::_argPos = 0;
Argument ArgumentParser::_args[MAX_ARGUMENTS];

Argument::Argument()
    : _cmd(NULL)
    , _callback(NULL)
{
    // Default constructor
}

Argument::~Argument()
{
    // Default destructor
}

/* Getters */
const char* Argument::getCommand() { return _cmd; }
cmdline_cb_t Argument::getCallback() { return _callback; }
/* Setters */
void Argument::setCommand(const char* cmd) { _cmd = cmd; }
void Argument::setCallback(cmdline_cb_t cb) { _callback = cb; }

ArgumentParser::ArgumentParser()
{
    // Default constructor
}

ArgumentParser::~ArgumentParser()
{
    // Default destructor
}

void ArgumentParser::parse(char* cmdline)
{
    _cmdline = cmdline;
    // Currently only arguments starting with `--` are supported
    // and are assumed to have a space separator between them
    char* currCmd = cmdline;
    Argument* arg = NULL;
    // This loop will stop once it reaches the string's null terminator
    while (currCmd) {
        // Skip over `-` characters and look for the arguments
        if (*currCmd == '-') {
            currCmd++;
            continue;
        }
        // Find argument takes the current command (which has progressed down
        // the string, skipping any `-` characters) and compares it against all
        // of the registered arguments, using ARGUMENT_DELIMETER as the delimiter
        // (this is currently a space character)
        if ((arg = findArgument(currCmd))) {
            // TODO: Find a way to pass the current argument back or some sort of
            //       handle that can be associated with the current argument. A void
            //       pointer to an int maybe? (Returned when registering and passed
            //       to the callback so the called function can determine which arg
            //       was found...)
            arg->getCallback()(NULL);
        }
    }
}

void ArgumentParser::registerArgument(const char* arg, cmdline_cb_t cb)
{
    Argument nextArg = ArgumentParser::_args[ArgumentParser::_argPos];
    ArgumentParser::_argPos++;
    nextArg.setCallback(cb);
    nextArg.setCommand(arg);
}

bool ArgumentParser::compareArguments(const char* str1, const char* str2, const char delim)
{
    // Keep in mind that const char* is desribing a pointer to a constant character,
    // and is not equivalent to a const char const *, which is a constant pointer to
    // a constant character. This is why wer are able to do pointer arithmetic here.
    while (*str1 == *str2) {
        if (*str1 == delim)
            return true;
        ++str1;
        ++str2;
    }

    return false;
}

Argument* ArgumentParser::findArgument(const char* arg)
{
    for (size_t i = 0; i < ArgumentParser::_argPos; i++) {
        Argument* nextArg = &ArgumentParser::_args[i];
        if (compareArguments(arg, nextArg->getCommand(), ARGUMENT_DELIMETER) == 0)
            return nextArg;
    }

    return NULL;
}

}
