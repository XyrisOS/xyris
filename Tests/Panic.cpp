#include <stdlib.h>
#include <Panic.hpp>
#include <catch2/catch.hpp>

[[noreturn]] void panic(const char* msg) {
    FAIL(
        "Panic while unit testing!\n" <<
        "Msg:\t" << msg << "\n"
    );

    __builtin_unreachable();
}

[[noreturn]] void panicf(const char* msg, ...) {
    FAIL(
        "Panic while unit testing!\n" <<
        "Msg:\t" << msg << "\n"
    );

    __builtin_unreachable();
}
