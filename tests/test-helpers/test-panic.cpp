#include <stdlib.h>
#include <arch/arch.hpp>
#include <catch2/catch.hpp>

// Provide a panic function for worse-case failure reporting
void panic(const char* msg, const char *file, uint32_t line, const char *func) {
    // Use the fail macro so that the error is recorded in the report XML.
    FAIL(
        "Panic while unit testing!\n" <<
        "Msg:\t" << msg <<
        "File:\t" << file << "\n"
        "Line:\t" << line << "\n"
        "Func:\t" << func << "\n"
    );
    // Force test case failure
    REQUIRE(0 == 1);
    // Force a no-return case
    // (TODO: Fix / rework this in the future?)
    exit(1);
}
