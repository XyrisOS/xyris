struc task
    .stack:     resd 1
    .page_dir:  resd 1
    .next_task: resd 1
    .state:     resd 1
    .time_used: resq 1
endstruc

%define TASK_RUNNING 0
%define TASK_READY   1

bits    32
section .text
extern  current_task:data, tasks_ready_tail:data
extern  _tasks_enqueue_ready:function
global  tasks_switch_to:function
tasks_switch_to:
    ;Save previous task's state
    ;Notes:
    ;  For cdecl; EAX, ECX, and EDX are already saved by the caller and don't need to be saved again
    ;  EIP is already saved on the stack by the caller's "CALL" instruction
    ;  The task isn't able to change CR3 so it doesn't need to be saved
    ;  Segment registers are constants (while running kernel code) so they don't need to be saved
    push ebx
    push esi
    push edi
    push ebp

    mov edi,[current_task]        ;edi = address of the previous task's "thread control block"
    mov [edi+task.stack],esp      ;Save ESP for previous task's kernel stack in the thread's TCB
    cmp dword [edi+task.state],TASK_RUNNING
    jne .state_updated
    mov dword [edi+task.state],TASK_READY ;Set the task's state to ready (from running)
    push edi ; push the task to be enqueued
    call _tasks_enqueue_ready ; call the enqueue function
    add esp,4 ; fix up the stack
 .state_updated:
    ;Load next task's state
    mov esi,[esp+(4+1)*4]         ;esi = address of the next task's "thread control block" (parameter passed on stack)
    mov [current_task],esi        ;Current task's TCB is the next task TCB

    mov esp,[esi+task.stack]      ;Load ESP for next task's kernel stack from the thread's TCB

    ; at this point we are operating under the new task's stack

    mov dword [esi+task.state],TASK_RUNNING ;this task is now running
    mov eax,[esi+task.page_dir]   ;eax = address of page directory for next task
    ;mov ebx,[esi+TCB.ESP0]       ;ebx = address for the top of the next task's kernel stack
    ;mov [TSS.ESP0],ebx           ;Adjust the ESP0 field in the TSS (used by CPU for for CPL=3 -> CPL=0 privilege level changes)
    mov ecx,cr3                   ;ecx = previous task's virtual address space

    cmp eax,ecx                   ;Does the virtual address space need to being changed?
    je .done_virt_addr            ; no, virtual address space is the same, so don't reload it and cause TLB flushes
    mov cr3,eax                   ; yes, load the next task's virtual address space
.done_virt_addr:
    pop ebp
    pop edi
    pop esi
    pop ebx
    ret                           ;Load next task's EIP from its kernel stack
