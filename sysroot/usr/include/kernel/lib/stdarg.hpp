/**
 * @file stdarg.hpp
 * @author Colin Peters (colin@bird.fu.is.saga-u.ac.jp)
 * & Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Standard arguments header. Used to specify
 * a variable amount of arguments in a function.
 * @version 0.3
 * @date 2020-07-09
 * 
 * @copyright This code is public domain (no copyright).
 * You can do whatever you want with it. Modified by
 * Keeton Feavel.
 * 
 */

/**
 * @brief Variable argument list macros necessary for functions like printf().
 * Created by Colin Peters <colin@bird.fu.is.saga-u.ac.jp> for Mingw under
 * public domain. Thanks for your hard work. Similar code can be found in any
 * C or C++ library implementation in the stdarg.h header.
 */
typedef char* va_list;
#define __va_argsiz(x) (((sizeof(x) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))
#define va_start(x, y) ((x) = ((va_list) __builtin_next_arg(y)))
#define va_end(x) ((void) 0)
#define va_arg(x, y) (((x) = (x) + __va_argsiz(y)), *((y *) (void *) ((x) - __va_argsiz(y))))