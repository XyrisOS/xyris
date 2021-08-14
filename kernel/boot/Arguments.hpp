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

namespace Boot {

typedef void (*cmdline_cb_t)(char* arg);

class Argument {
public:
    Argument();
    ~Argument();
    /* Getters */
    const char* getCommand();
    cmdline_cb_t getCallback();
    /* Setters */
    void setCommand(const char* cmd);
    void setCallback(cmdline_cb_t cb);

private:
    const char* _cmd;
    cmdline_cb_t _callback;
};

class ArgumentParser {
public:
    ArgumentParser();
    ~ArgumentParser();
    void parse(char* cmdline);

    /**
     * @brief Register an argument and callback with the argument parser
     *
     * @param arg Argument to be matched. Do not include the `--`.
     * @param cb Callback to be called if a match is found
     */
    static void registerArgument(const char* arg, cmdline_cb_t cb);

private:
    bool compareArguments(const char* str1, const char* str2, const char delim);
    Argument* findArgument(const char* arg);

    static char* _cmdline;
    static size_t _argPos;
    // TODO: Add a vector library
    static Argument _args[MAX_ARGUMENTS];
};

}
