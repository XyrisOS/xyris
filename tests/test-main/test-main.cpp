/**
 * @file main.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Unit Test Entry Point
 * @version 0.1
 * @date 2021-07-03
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
// Let Catch provide main():
#define CATCH_CONFIG_MAIN
// Include Catch2 single header
#include <catch2/catch.hpp>
// Function prototypes
void panic(const char* msg, const char *file, uint32_t line, const char *func);

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
}
