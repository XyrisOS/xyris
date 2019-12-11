/**
 * @file types.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Defines the shorthand names for their equivalent variable types.
 * @version 0.1
 * @date 2019-09-26
 * 
 * @copyright Copyright Keeton Feavel (c) 2019
 * 
 */

#ifndef PANIX_TYPES_HPP
#define PANIX_TYPES_HPP

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

typedef struct registers {
   uint32_t ds;                                          /* Data segment selector */
   uint32_t edi, esi, ebp, ignored, ebx, edx, ecx, eax;  /* Pushed by pusha. */
   uint32_t int_num, err_code;                           /* Interrupt number and error code (if applicable) */
   uint32_t eip, cs, eflags, esp, ss;                    /* Pushed by the processor automatically */
} registers_t;
typedef void (*isr_t)(registers_t);
    
#endif /* PANIX_TYPES_HPP */