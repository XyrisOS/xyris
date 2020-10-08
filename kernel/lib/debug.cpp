/**
 * @file debug.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief 
 * @version 0.1
 * @date 2020-10-07
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */

#include <lib/stdio.hpp>
#include <mem/heap.hpp>
#include <dev/serial/rs232.hpp>

int px_debugf(const char* fmt, ...) {
    // Return counter
    int ret = 0;
#if defined(DEBUG)
    // Create the arguments list
    va_list args;
    va_start(args, fmt);
    // Statically allocate a print buffer
    // since we may call this before we
    // have paging available.
    char buf[1024];
    // Write the string to the buffer
    ret = px_kvsprintf(buf, fmt, args);
    // Print to serial
    px_rs232_print(buf);
#else
    (void)fmt;
#endif
    return ret;
}
