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

%if 1
section .early_data
align 4
__stivale2_framebuffer:
    dq 0x3ecc1bc43d0f7971
    dq 0
    dw 1280
    dw 720
    dw 32
%endif
