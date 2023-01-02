/**
 * @file printf.cpp
 * @author Chris Giese (geezer@execpc.com)
 * @author Keeton Feavel (keetonfeavel@cedaville.edu)
 * @brief A printf implementation released under the public domain.
 * @version 0.3
 * @date 2020-07-09
 *
 * @copyright This code is public domain (no copyright).
 * You can do whatever you want with it. Modified by
 * Keeton Feavel.
 *
 * http://www.osdev.labedz.org/src/lib/stdio/printf.c
 *
 */
#include <Library/stdio.hpp>
#include <Library/string.hpp>
#include <limits.h>
#include <stdarg.h>

/*****************************************************************************
Stripped-down printf()
Chris Giese <geezer@execpc.com>    http://www.execpc.com/~geezer
Release date: Dec 12, 2003
This code is public domain (no copyright).
You can do whatever you want with it.

Revised Dec 12, 2003
- fixed vsprintf() and sprintf() in test code

Revised Jan 28, 2002
- changes to make characters 0x80-0xFF display properly

Revised June 10, 2001
- changes to make vsprintf() terminate string with '\0'

Revised May 12, 2000
- math in DO_NUM is now unsigned, as it should be
- %0 flag (pad left with zeroes) now works
- actually did some TESTING, maybe fixed some other bugs

%[flag][width][.prec][mod][conv]
flag:
    -           left justify, pad right w/ blanks       DONE
    0           pad left w/ 0 for numerics              DONE
    +           always print sign, + or -               no
    ' '         (blank)                                 no
    #           (???)                                   no

width:          (field width)                           DONE

prec:           (precision)                             no

conv:
    d,i         decimal int                             DONE
    u           decimal unsigned                        DONE
    o           octal                                   DONE
    x,X         hex                                     DONE
    f,e,g,E,G   float                                   no
    c           char                                    DONE
    s           string                                  DONE
    p           ptr                                     DONE
    z           size_t                                  DONE

mod:
    N           near ptr                                DONE
    F           far ptr                                 no
    h           short (16-bit) int                      DONE
    l           long (32-bit) int                       DONE
    L           long long (64-bit) int                  DONE
*****************************************************************************/

/* Assume: width of stack == width of int. Don't use sizeof(char *) or
other pointer because sizeof(char *)==4 for LARGE-model 16-bit code.
Assume: width is a power of 2 */
#define STACK_WIDTH sizeof(int)

/* Round up object width so it's an even multiple of STACK_WIDTH.
Using & for division here, so STACK_WIDTH must be a power of 2. */
#define TYPE_WIDTH(TYPE) ((sizeof(TYPE) + STACK_WIDTH - 1) & ~(STACK_WIDTH - 1))

/* flags used in processing format string */
enum printf_state {
    PR_LJ = 0b000000001, /* left justify */
    PR_CA = 0b000000010, /* use A-F instead of a-f for hex */
    PR_SG = 0b000000100, /* signed numeric conversion (%d vs. %u) */
    PR_64 = 0b000001000, /* long long (64-bit) numeric conversion*/
    PR_32 = 0b000010000, /* long (32-bit) numeric conversion */
    PR_16 = 0b000100000, /* short (16-bit) numeric conversion */
    PR_WS = 0b001000000, /* PR_SG set and num was < 0 */
    PR_LZ = 0b010000000, /* pad left with '0' instead of ' ' */
    PR_FP = 0b100000000, /* pointers are far */
};
/* largest number handled is 2^32-1, lowest radix handled is 8.
2^64-1 in base 8 has 22 digits (add 5 for trailing NUL and for slop)
round to 32 so that it's on a byte boundary */
#define PR_BUFLEN 32

/*****************************************************************************
name:    printf_helper
action:    minimal subfunction for ?printf, calls function
    'fn' with arg 'ptr' for each character to be output
returns:total number of characters output
*****************************************************************************/
int printf_helper(const char* fmt, va_list args, printf_cb_fnptr_t fn, void* ptr)
{
    unsigned char radix, *where, buf[PR_BUFLEN];
    unsigned int state, flags, actual_wd, count, given_wd;
    long num;

    state = flags = count = given_wd = 0;
    /* begin scanning format specifier list */
    for (; *fmt; fmt++) {
        switch (state) {
                /* STATE 0: AWAITING % */
            case 0:
                if (*fmt != '%') /* not %... */
                {
                    fn(*fmt, &ptr); /* ...just echo it */
                    count++;
                    break;
                }
                /* found %, get next char and advance state to check if next char is a flag */
                state++;
                fmt++;
                /* STATE 1: AWAITING FLAGS (%-0) */
                [[fallthrough]];
            case 1:
                if (*fmt == '%') /* %% */
                {
                    fn(*fmt, &ptr);
                    count++;
                    state = flags = given_wd = 0;
                    break;
                }
                if (*fmt == '-') {
                    if (flags & PR_LJ) /* %-- is illegal */
                        state = flags = given_wd = 0;
                    else
                        flags |= PR_LJ;
                    break;
                }
                /* not a flag char: advance state to check if it's field width */
                state++;
                /* check now for '%0...' */
                if (*fmt == '0') {
                    flags |= PR_LZ;
                    fmt++;
                }
                /* STATE 2: AWAITING (NUMERIC) FIELD WIDTH */
                [[fallthrough]];
            case 2:
                if (*fmt >= '0' && *fmt <= '9') {
                    given_wd = 10 * given_wd + (*fmt - '0');
                    break;
                }
                /* not field width: advance state to check if it's a modifier */
                state++;
                /* STATE 3: AWAITING MODIFIER CHARS (FNlh) */
                [[fallthrough]];
            case 3:
                if (*fmt == 'F') {
                    flags |= PR_FP;
                    break;
                }
                if (*fmt == 'N') {
                    break;
                }
                if (*fmt == 'z') {
#if (UINTPTR_MAX == UINT32_MAX)
                    flags |= PR_32;
#elif (UINTPTR_MAX == UINT64_MAX)
                    flags |= PR_64;
#else
#error "Unknown UINTPTR_MAX value! Cannot compile printf_helper!"
#endif
                    break;
                }
                if (*fmt == 'l') {
                    flags |= PR_32;
                    break;
                }
                if (*fmt == 'L') {
                    flags |= PR_64;
                    break;
                }
                if (*fmt == 'h') {
                    flags |= PR_16;
                    break;
                }
                /* not modifier: advance state to check if it's a conversion char */
                state++;
                /* STATE 4: AWAITING CONVERSION CHARS (Xxpndiuocs) */
                [[fallthrough]];
            case 4: {
                where = buf + PR_BUFLEN - 1;
                *where = '\0';
                switch (*fmt) {
                    case 'X':
                        flags |= PR_CA;
                        /* xxx - far pointers (%Fp, %Fn) not yet supported */
                        [[fallthrough]];
                    case 'x':
                    case 'p':
                        /* pointers should be padded with '0' */
                        flags |= PR_LZ;
                        /* set width of output based on size of pointer */
                        given_wd = sizeof(size_t) * 2;
#if (UINTPTR_MAX == UINT32_MAX)
                        flags |= PR_32;
#elif (UINTPTR_MAX == UINT64_MAX)
                        flags |= PR_64;
#else
#error "Unknown UINTPTR_MAX value! Cannot compile printf_helper!"
#endif
                        [[fallthrough]];
                    case 'n':
                        radix = 16;
                        goto DO_NUM;
                    case 'd':
                    case 'i':
                        flags |= PR_SG;
                        [[fallthrough]];
                    case 'u':
                        radix = 10;
                        goto DO_NUM;
                    case 'o':
                        radix = 8;
                    /* load the value to be printed. l=long=32 bits: */
                    DO_NUM:
                        if (flags & PR_64)
                            num = va_arg(args, unsigned long long);
                        else if (flags & PR_32)
                            num = va_arg(args, unsigned long);
                        /* h=short=16 bits (signed or unsigned) */
                        else if (flags & PR_16) {
                            if (flags & PR_SG) {
                                num = va_arg(args, int);
                            } else {
                                num = va_arg(args, int);
                            }
                        }
                        /* no h nor l: sizeof(int) bits (signed or unsigned) */
                        else {
                            if (flags & PR_SG) {
                                num = va_arg(args, int);
                            } else {
                                num = va_arg(args, unsigned int);
                            }
                        }
                        /* take care of sign */
                        if (flags & PR_SG) {
                            if (num < 0) {
                                flags |= PR_WS;
                                num = -num;
                            }
                        }
                        /* convert binary to octal/decimal/hex ASCII
                        OK, I found my mistake. The math here is _always_ unsigned */
                        do {
                            unsigned long temp;

                            temp = (unsigned long)num % radix;
                            where--;
                            if (temp < 10) {
                                *where = (unsigned char)(temp + '0');
                            } else if (flags & PR_CA) {
                                *where = (unsigned char)(temp - 10 + 'A');
                            } else {
                                *where = (unsigned char)(temp - 10 + 'a');
                            }
                            num = (unsigned long)num / radix;
                        } while (num != 0);
                        goto EMIT;
                    case 'c':
                        /* disallow pad-left-with-zeroes for %c */
                        flags &= ~PR_LZ;
                        where--;
                        *where = (unsigned char)va_arg(args, int);
                        actual_wd = 1;
                        goto EMIT2;
                    case 's':
                        /* disallow pad-left-with-zeroes for %s */
                        flags &= ~PR_LZ;
                        where = va_arg(args, unsigned char*);
                    EMIT:
                        actual_wd = strlen((const char*)where);
                        if (flags & PR_WS) {
                            actual_wd++;
                        }
                        /* if we pad left with ZEROES, do the sign now */
                        if ((flags & (PR_WS | PR_LZ)) == (PR_WS | PR_LZ)) {
                            fn('-', &ptr);
                            count++;
                        }
                    /* pad on left with spaces or zeroes (for right justify) */
                    EMIT2:
                        if ((flags & PR_LJ) == 0) {
                            while (given_wd > actual_wd) {
                                fn(flags & PR_LZ ? '0' : ' ', &ptr);
                                count++;
                                given_wd--;
                            }
                        }
                        /* if we pad left with SPACES, do the sign now */
                        if ((flags & (PR_WS | PR_LZ)) == PR_WS) {
                            fn('-', &ptr);
                            count++;
                        }
                        /* emit string/char/converted number */
                        while (*where != '\0') {
                            fn(*where++, &ptr);
                            count++;
                        }
                        /* pad on right with spaces (for left justify) */
                        if (given_wd < actual_wd) {
                            given_wd = 0;
                        } else {
                            given_wd -= actual_wd;
                        }
                        for (; given_wd; given_wd--) {
                            fn(' ', &ptr);
                            count++;
                        }
                        break;
                    default:
                        break;
                }
                // Tell the compiler that this isn't an issue (at least I'm assuming it isn't)
                [[fallthrough]];
            }
            default:
                state = flags = given_wd = 0;
                break;
        }
    }
    return count;
}

static int vsprintf_help(unsigned c, void** ptr)
{
    char* dst;

    dst = (char*)*ptr;
    *dst++ = (char)c;
    *ptr = dst;
    return 0;
}

int kvsprintf(char* buf, const char* fmt, va_list args)
{
    int ret_val;

    ret_val = printf_helper(fmt, args, vsprintf_help, (void*)buf);
    buf[ret_val] = '\0';
    return ret_val;
}

int ksprintf(char* buf, const char* fmt, ...)
{
    va_list args;
    int ret_val;

    va_start(args, fmt);
    ret_val = kvsprintf(buf, fmt, args);
    va_end(args);
    return ret_val;
}
