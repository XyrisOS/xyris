section .text
align 4

global invalidate_page
invalidate_page:
    mov    eax, [esp+4]
    invlpg  [eax]
    ret
