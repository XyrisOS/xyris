extern _EARLY_BSS_END:data
extern _start:code

STIVALE2_MAGIC equ 'stv2'

section .early_text
__stivale2_entry:
    mov ebx, [esp + 4]
    mov eax, STIVALE2_MAGIC
    jmp _start

section .stivale2hdr
align 4
__stivale2_header:
    dd __stivale2_entry ; entry
    dd 0 ; entry, high bits
    dd _EARLY_BSS_END
    dd 0 ; stack, high bits
    dq 0 ; flags
    ; tags
    dd __stivale2_framebuffer
    dd 0

section .early_data
align 4
__stivale2_framebuffer:
    dq 0x3ecc1bc43d0f7971
    dd __stivale2_cmdline
    dw 0 ; Width (0 = Best)
    dw 0 ; Height (0 = Best)
    dw 0 ; Color depth (0 = Best)

align 4
__stivale2_cmdline:
    dq 0xe5e76a1b4597a781
    dq 0
    dq 0
