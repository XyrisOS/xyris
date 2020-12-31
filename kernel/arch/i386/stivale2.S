extern _EARLY_BSS_END:data

section .stivale2hdr
align 4
__stivale2_header:
    dq 0 ; entry 
    dd _EARLY_BSS_END
    dd 0 ; stack, high bits
    dq 0 ; flags
    dq 0 ; tags
