MULTIBOOT_MAGIC      equ 0x1badb002
MULTIBOOT_PAGE_ALIGN equ 1<<0
MULTIBOOT_MEM_INFO   equ 1<<1
MULTIBOOT_FLAGS      equ (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEM_INFO)
MULTIBOOT_CHECKSUM   equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

; Multiboot header section of our executable. See linker.ld
section .multiboot
__multiboot_header:
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM
