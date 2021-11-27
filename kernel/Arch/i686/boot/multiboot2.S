MULTIBOOT_MAGIC     equ 0xE85250D6
MULTIBOOT_ARCH      equ 0
MULTIBOOT_LENGTH    equ (__multiboot_header_end - __multiboot2_header_start)
MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_MAGIC + MULTIBOOT_ARCH + MULTIBOOT_LENGTH)

MULTIBOOT_TAG_OPTIONAL equ 1
MULTIBOOT_TAG_ENTRY equ 3
MULTIBOOT_TAG_FRAMEBUFFER equ 5
MULTIBOOT_TAG_LAST  equ 0

extern _start:code
MULTIBOOT_ENTRY equ _start

%macro tag_begin 1-2 0
align 8
.start:
    dw %1
    dw %2
    dd (.end - .start)
%endmacro

%macro tag_end 0
.end:
%endmacro

section .multiboot2
align 8
__multiboot2_header_start:
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_ARCH
    dd MULTIBOOT_LENGTH
    dd MULTIBOOT_CHECKSUM

__multiboot2_entry_tag:
    tag_begin MULTIBOOT_TAG_ENTRY, 1
    dd  MULTIBOOT_ENTRY
    tag_end

__multiboot2_framebuffer_tag:
    tag_begin MULTIBOOT_TAG_FRAMEBUFFER
    dw MULTIBOOT_TAG_OPTIONAL
    dd 20
    dd 0
    dd 0
    dd 32
    tag_end

__multiboot2_last_tag:
    tag_begin MULTIBOOT_TAG_LAST
    tag_end

__multiboot_header_end:
