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

#define __YEAR_C__ 7
#define __YEAR__  (\
                    ((__DATE__)[__YEAR_C__ + 0] - '0') * 1000 + \
                    ((__DATE__)[__YEAR_C__ + 1] - '0') * 100  + \
                    ((__DATE__)[__YEAR_C__ + 2] - '0') * 10   + \
                    ((__DATE__)[__YEAR_C__ + 3] - '0') * 1      \
                  )

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

#endif /* PANIX_TYPES_HPP */
