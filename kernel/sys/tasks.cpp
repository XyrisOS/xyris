/**
 * @file tasks.cpp
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @brief 
 * @version 0.3
 * @date 2020-08-29
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */
#include <sys/tasks.hpp>
#include <mem/heap.hpp>
#include <sys/panic.hpp>
#include <stdint.h>    // Data type definitions

px_task_t *px_current_task = NULL;

void px_tasks_init()
{
    // allocate memory for our task structure
    px_task_t *this_task = (px_task_t*)malloc(sizeof(px_task_t));
    // panic if the alloc fails (we have no fallback)
    if (this_task == NULL) PANIC("Unable to allocate memory for new task struct.\n");
    *this_task = {
        // this will be filled in when we switch to another task for the first time
        .stack_top = 0,
        // this will be the same for kernel tasks
        .page_dir = px_get_phys_page_dir(),
        // there are no other tasks yet
        .next_task = NULL,
        // this task is currently running
        .state = TASK_RUNNING
    };
    // this is the current task
    px_current_task = this_task;
}

static void _px_tasks_task_starting()
{
    asm("nop");
    // this is called whenever a new task is about to start
    // it is run in the context of the new task
}

// emulate a stack push
// this makes it easier and more intuitive to set up stacks
static inline void _px_stack_push_word(void **stack_pointer, size_t value)
{
    // decrement the stack pointer
    *(uintptr_t*)stack_pointer -= sizeof(size_t);
    // place the new value at that new address
    **(size_t**)stack_pointer = value;
}

px_task_t *px_tasks_new(void (*entry)(void))
{
    // allocate memory for our task structure
    px_task_t *new_task = (px_task_t*)malloc(sizeof(px_task_t));
    // panic if the alloc fails (we have no fallback)
    if (new_task == NULL) PANIC("Unable to allocate memory for new task struct.\n");
    // allocate a page for this stack (we might change this later)
    uint8_t *stack = (uint8_t *)px_get_new_page(PAGE_SIZE - 1);
    if (stack == NULL) PANIC("Unable to allocate memory for new task stack.\n");
    // remember, the stack grows up
    void *stack_pointer = stack + PAGE_SIZE;
    // last entry is the final function to call (the start of the task)
    _px_stack_push_word(&stack_pointer, (size_t)entry);
    // when this task is started, the CPU will pop off this value which will become the new EIP
    // we push this function to allow some setup code to be run from within the context of the new task
    _px_stack_push_word(&stack_pointer, (size_t)_px_tasks_task_starting);
    // our task switching code is going to pop four values off of the stack before returning
    _px_stack_push_word(&stack_pointer, 0);
    _px_stack_push_word(&stack_pointer, 0);
    _px_stack_push_word(&stack_pointer, 0);
    _px_stack_push_word(&stack_pointer, 0);
    *new_task = {
        .stack_top = (uintptr_t)stack_pointer,
        .page_dir = px_get_phys_page_dir(),
        .next_task = NULL,
        .state = TASK_PAUSED
    };
    return new_task;
}
