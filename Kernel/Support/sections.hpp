/**
 * @file sections.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Kernel ELF section definitions
 * @version 0.1
 * @date 2021-06-27
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#pragma once

#include <stddef.h>

// Kernel ELF Sections
// Externs (address values) are prefixed with an underscore
// Pointers to sections have the underscore removed
extern size_t _EARLY_MEM_START;
#define EARLY_MEM_START ((uintptr_t)&_EARLY_MEM_START)

extern size_t _EARLY_KERNEL_START;
#define EARLY_KERNEL_START ((uintptr_t)&_EARLY_KERNEL_START)
extern size_t _EARLY_KERNEL_END;
#define EARLY_KERNEL_END ((uintptr_t)&_EARLY_KERNEL_END)

extern size_t _KERNEL_BASE;
#define KERNEL_BASE ((uintptr_t)&_KERNEL_BASE)

extern size_t _KERNEL_SIZE;
#define KERNEL_SIZE ((size_t)&_KERNEL_SIZE)
extern size_t _KERNEL_START;
#define KERNEL_START ((uintptr_t)&_KERNEL_START)
extern size_t _KERNEL_END;
#define KERNEL_END ((uintptr_t)&_KERNEL_END)

extern size_t _EARLY_BSS_SIZE;
extern size_t _EARLY_BSS_START;
#define EARLY_BSS_START ((uintptr_t)&_EARLY_BSS_START)
extern size_t _EARLY_BSS_END;
#define EARLY_BSS_END ((uintptr_t)&_EARLY_BSS_END)

extern size_t _CTORS_START;
#define CTORS_START ((uintptr_t)&_CTORS_START)
extern size_t _CTORS_END;
#define CTORS_END ((uintptr_t)&_CTORS_END)

extern size_t _BSS_SIZE;
extern size_t _BSS_START;
#define BSS_START ((uintptr_t)&_BSS_START)
extern size_t _BSS_END;
#define BSS_END ((uintptr_t)&_BSS_END)

extern size_t _PAGE_TABLES_START;
#define PAGE_TABLES_START ((uintptr_t)&_PAGE_TABLES_START)
extern size_t _PAGE_TABLES_END;
#define PAGE_TABLES_END ((uintptr_t)&_PAGE_TABLES_END)
