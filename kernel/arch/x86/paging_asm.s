.section .text
.align 4

.global px_invalidate_page
px_invalidate_page:
#if TARGET_MACHINE >= TARGET_MACHINE_I486
    movl    4(%esp),%eax
    invlpg  (%eax)
#else   /* TARGET_MACHINE < TARGET_MACHINE_I486 */
    movl    %cr3,%eax
    movl    %eax,%cr3
#endif
    ret

