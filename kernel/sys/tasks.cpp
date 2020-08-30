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
// this is a literal hack to specifically circumvent validation checks
// it seems to work fine for the specific intrinsic we need
// we should probably investigate this further at some point...
#define _X86INTRIN_H_INCLUDED
#include <ia32intrin.h> // needed for __rdtsc
#undef _X86INTRIN_H_INCLUDED

px_task_t *px_current_task = NULL;
px_task_t *px_tasks_ready_head = NULL;
px_task_t *px_tasks_ready_tail = NULL;

static uint64_t _last_time = 0;
static size_t _scheduler_lock = 0;

static void _aquire_scheduler_lock()
{
    asm volatile("cli");
    _scheduler_lock++;
}

static void _release_scheduler_lock()
{
    _scheduler_lock--;
    if (_scheduler_lock == 0) {
        asm volatile("sti");
    }
}

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
        // this is a linked list with only this task 
        .next_task = NULL,
        // this task is currently running
        .state = TASK_RUNNING,
        // just say that this task hasn't spent any time running yet
        .time_used = 0
    };
    _last_time = __rdtsc();
    // this is the current task
    px_current_task = this_task;
}

static void _px_tasks_task_starting()
{
    // this is called whenever a new task is about to start
    // it is run in the context of the new task

    // the task before this caused the scheduler to lock
    _release_scheduler_lock();
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

extern "C" void _px_tasks_enqueue_ready(px_task_t *task)
{
    if (px_tasks_ready_head == NULL) {
        px_tasks_ready_head = task;
    }
    if (px_tasks_ready_tail != NULL) {
        // the current last task's next pointer will be this task
        px_tasks_ready_tail->next_task = task;
    }
    // and now this task becomes the last task
    px_tasks_ready_tail = task;
}

static px_task_t *_px_tasks_dequeue_ready()
{
    px_task_t *task;
    if (px_tasks_ready_head == NULL) {
        // can't dequeue if there's not anything there
        return NULL;
    }
    // the head of the list is the next item
    task = px_tasks_ready_head;
    // the new head is the next task
    px_tasks_ready_head = task->next_task;
    if (px_tasks_ready_head == NULL) {
        // if there are no more items in the list, then
        // the last item in the list will also be null
        px_tasks_ready_tail = NULL;
    }
    // it doesn't make sense to have a next_task when it's not in a list
    task->next_task = NULL;
    return task;
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
        .state = TASK_READY,
        .time_used = 0
    };
    _px_tasks_enqueue_ready(new_task);
    return new_task;
}

void px_tasks_update_time()
{
    uint64_t current_time = __rdtsc();
    uint64_t delta = current_time - _last_time;
    px_current_task->time_used += delta;
    _last_time = current_time;
}

void px_tasks_schedule()
{
    // we must lock on all scheduling operations
    _aquire_scheduler_lock();
    // get the next task
    px_task_t *task = _px_tasks_dequeue_ready();
    // don't need to do anything if there's nothing ready to run
    if (task == NULL) goto done;
    // do time accounting
    px_tasks_update_time();
    // switch to the task
    px_tasks_switch_to(task);
done:
    // this will run when we switch back to the calling task
    _release_scheduler_lock();
}

uint64_t px_tasks_get_self_time()
{
    px_tasks_update_time();
    return px_current_task->time_used;
}
