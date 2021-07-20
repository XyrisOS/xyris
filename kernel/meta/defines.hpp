/**
 * @file defines.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Compiler pre-processor definitions
 * @version 0.1
 * @date 2021-07-20
 *
 * @copyright Copyright the Panix Contributors (c) 2021
 *
 */
#pragma once

// Define the Git commit version if not declared by compiler
#ifndef COMMIT
    #define COMMIT "unknown"
#endif
// Assume that if major isn't set none of them are
#ifndef VER_MAJOR
    #define VER_MAJOR '0'
    #define VER_MINOR '0'
    #define VER_PATCH '0'
#endif
// Panix release name
#ifndef VER_NAME
    #define VER_NAME "unknown"
#endif
#ifndef REPO_URL
    #define REPO_URL "(unknown)"
#endif
