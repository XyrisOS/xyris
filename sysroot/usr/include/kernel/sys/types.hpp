/**
 * @file types.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Defines the shorthand names for their equivalent variable types.
 * @version 0.3
 * @date 2019-09-26
 *
 * @copyright Copyright Keeton Feavel (c) 2019
 *
 */

#ifndef PANIX_TYPES_HPP
#define PANIX_TYPES_HPP

#define NULL nullptr
#define typeof __typeof__

typedef char                     int8_t;
typedef unsigned char           uint8_t;
typedef short                   int16_t;
typedef unsigned short         uint16_t;
typedef int                     int32_t;
typedef unsigned int           uint32_t;
typedef long long int           int64_t;
typedef unsigned long long int uint64_t;
typedef unsigned int             size_t;
typedef unsigned long         uintptr_t;
typedef long                   intptr_t;
typedef char *                  va_list;

/**
 * @brief Variable argument list macros necessary for functions like printf().
 * Created by Colin Peters <colin@bird.fu.is.saga-u.ac.jp> for Mingw under
 * public domain. Thanks for your hard work.
 */
#define __va_argsize(x) (((sizeof(x) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))
#define va_start(x, y) ((x) = ((va_list) __builtin_next_arg(y)))
#define va_end(x) ((void) 0)
#define va_arg(x, y) (((x) = (x) + __va_argsiz(y)), *((y *) (void *) ((x) - __va_argsiz(y))))

#endif /* PANIX_TYPES_HPP */
