/**
 * @file errno.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Kernel error definitions
 * @version 0.3
 * @date 2020-08-08
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#pragma once

/**
 * @brief Kernel error codes. All values other than `OK` should be negative.
 *
 */
enum KernelError {
    LockTaken = -4,
    BufferFull = -3,
    InvalidValue = -2,
    GenericError = -1,
    OK = 0,
};

// FIXME: This is currently not thread safe.
extern KernelError errno;
