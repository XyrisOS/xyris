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
#include <lib/stdio.hpp>

#include <stdint.h>    // Data type definitions
// this is a literal hack to specifically circumvent validation checks
// it seems to work fine for the specific intrinsic we need
// we should probably investigate this further at some point...
#define _X86INTRIN_H_INCLUDED
#include <ia32intrin.h> // needed for __rdtsc
#undef _X86INTRIN_H_INCLUDED

#include <arch/arch.hpp>

px_task_t *px_current_task = NULL;

px_tasklist_t px_tasks_ready = { 0 };
px_tasklist_t px_tasks_sleeping = { 0 };

static uint64_t _idle_time = 0;
static uint64_t _idle_start = 0;
static uint64_t _last_time = 0;
static size_t _scheduler_lock = 0;
static uint64_t _instr_per_ns;

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

static void _discover_cpu_speed()
{
    uint32_t curr_tick = px_timer_tick;
    uint64_t curr_rtsc = __rdtsc();
    while (px_timer_tick != curr_tick + 1) { }
    curr_rtsc = __rdtsc() - curr_rtsc;
    _instr_per_ns = curr_rtsc / 1000000;
    // will be inaccurate, but it's the best we can do in these circumstances
    if (_instr_per_ns == 0) _instr_per_ns = 1;
}

static inline uint64_t _get_cpu_time_ns()
{
    return (__rdtsc()) / _instr_per_ns;
}

static void _on_timer();

void px_tasks_init()
{
    // allocate memory for our task structure
    px_task_t *this_task = (px_task_t*)malloc(sizeof(px_task_t));
    // panic if the alloc fails (we have no fallback)
    if (this_task == NULL) PANIC("Unable to allocate memory for new task struct.\n");
    // discover the CPU speed for accurate scheduling
    _discover_cpu_speed();
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
    _last_time = _get_cpu_time_ns();
    // this is the current task
    px_current_task = this_task;
    px_timer_register_callback(_on_timer);
}

static void _task_starting()
{
    // this is called whenever a new task is about to start
    // it is run in the context of the new task

    // the task before this caused the scheduler to lock
    _release_scheduler_lock();
}

static void _task_stopping()
{
    // this is called whenever a task is about to stop (i.e. it returned)
    // it is run in the context of the stopping task
    px_tasks_block_current(TASK_STOPPED);
    // prevent undefined behavior from returning to a random address
    PANIC("Attempted to schedule a stopped task\n");
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

static void _enqueue_task(px_tasklist_t *list, px_task *task)
{
    if (list->head == NULL) {
        list->head = task;
    }
    if (list->tail != NULL) {
        // the current last task's next pointer will be this task
        list->tail->next_task = task;
    }
    // and now this task becomes the last task
    list->tail = task;
}

static px_task_t *_dequeue_task(px_tasklist_t *list)
{
    px_task_t *task;
    if (list->head == NULL) {
        // can't dequeue if there's not anything there
        return NULL;
    }
    // the head of the list is the next item
    task = list->head;
    // the new head is the next task
    list->head = task->next_task;
    if (list->head == NULL) {
        // if there are no more items in the list, then
        // the last item in the list will also be null
        list->tail = NULL;
    }
    // it doesn't make sense to have a next_task when it's not in a list
    task->next_task = NULL;
    return task;
}

static void _remove_task(px_tasklist_t *list, px_task_t *task, px_task_t *previous)
{
    // if this is true, something's not right...
    if (previous != NULL && previous->next_task != task) {
        PANIC("Bogus arguments to _remove_task.\n");
    }
    // update the head if necessary
    if (list->head == task) {
        list->head = task->next_task;
    }
    // update the tail if necessary
    if (list->tail == task) {
        list->tail = previous;
    }
    // update the previous task if necessary
    if (previous != NULL) {
        previous->next_task = task->next_task;
    }
    // it's not in any list anymore, so clear its next pointer
    task->next_task = NULL;
}

extern "C" void _px_tasks_enqueue_ready(px_task_t *task)
{
    _enqueue_task(&px_tasks_ready, task);
}

static px_task_t *_px_tasks_dequeue_ready()
{
    return _dequeue_task(&px_tasks_ready);
}

static void _enqueue_sleeping(px_task_t *task)
{
    _enqueue_task(&px_tasks_sleeping, task);
}

static px_task_t *_dequeue_sleeping()
{
    return _dequeue_task(&px_tasks_sleeping);
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
    // a null stack frame to make the panic screen happy
    _px_stack_push_word(&stack_pointer, 0);
    // the last thing to happen is the task stopping function
    _px_stack_push_word(&stack_pointer, (size_t)_task_stopping);
    // next entry is the main function to call (the start of the task)
    _px_stack_push_word(&stack_pointer, (size_t)entry);
    // when this task is started, the CPU will pop off this value which will become the new EIP
    // we push this function to allow some setup code to be run from within the context of the new task
    _px_stack_push_word(&stack_pointer, (size_t)_task_starting);
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
    uint64_t current_time = _get_cpu_time_ns();
    uint64_t delta = current_time - _last_time;
    if (px_current_task == NULL) {
        _idle_time += delta;
    } else {
        px_current_task->time_used += delta;
    }
    _last_time = current_time;
}

static void _schedule()
{
    if (px_current_task == NULL) {
        // we are currently idling and will schedule at a later time
        return;
    }
    // get the next task
    px_task_t *task = _px_tasks_dequeue_ready();
    // don't need to do anything if there's nothing ready to run
    if (task == NULL) {
        if (px_current_task->state == TASK_RUNNING) {
            // still running the same task
            return;
        }
        // count the time that this task ran for
        px_tasks_update_time();
        /*** idle ***/
        // borrow this task to return to once we're not idle anymore
        px_task_t *borrowed = px_current_task;
        // set the current task to null to indicate an idle state
        px_current_task = NULL;
        //_idle_start = _get_cpu_time_ns();
        do {
            // enable interrupts to process timer and other events
            asm ("sti");
            // immediately halt the CPU
            asm ("hlt");
            // disable interrupts to restore our lock
            asm ("cli");
            // check if there's a task ready to be run
        } while(task = _px_tasks_dequeue_ready(), task == NULL);
        // count the time we spent idling
        px_tasks_update_time();
        // reset the current task
        px_current_task = borrowed;
        //_idle_start = _idle_start - _get_cpu_time_ns();
        //px_kprintf("Slept for %lld nanoseconds.\n", _idle_start);
    } else {
        // just do time accounting once
        px_tasks_update_time();
    }
    // switch to the task
    px_tasks_switch_to(task);
}

void px_tasks_schedule()
{
    // we must lock on all scheduling operations
    _aquire_scheduler_lock();
    // run the scheduler
    _schedule();
    // this will run when we switch back to the calling task
    _release_scheduler_lock();
}

uint64_t px_tasks_get_self_time()
{
    px_tasks_update_time();
    return px_current_task->time_used;
}

void px_tasks_block_current(px_task_state reason)
{
    _aquire_scheduler_lock();
    px_current_task->state = reason;
    _schedule();
    _release_scheduler_lock();
}

void px_tasks_unblock(px_task_t *task)
{
    _aquire_scheduler_lock();
    if (px_tasks_ready.head == NULL) {
        px_tasks_switch_to(task);
    } else {
        _px_tasks_enqueue_ready(task);
    }
    _release_scheduler_lock();
}

void _wakeup(px_task_t *task)
{
    task->state = TASK_READY;
    task->wakeup_time = (0ULL - 1);
    _px_tasks_enqueue_ready(task);
}

static void _on_timer()
{
    _aquire_scheduler_lock();
    px_task_t *pre = NULL;
    px_task_t *task = px_tasks_sleeping.head;
    bool need_schedule = false;
    uint64_t time = _get_cpu_time_ns();
    while (task != NULL) {
        if (time >= task->wakeup_time) {
            _remove_task(&px_tasks_sleeping, task, pre);
            _wakeup(task);
            need_schedule = true;
        }
        pre = task;
        task = task->next_task;
    }
    if (need_schedule) {
        _schedule();
    }
    _release_scheduler_lock();
}

void px_tasks_nano_sleep_until(uint64_t time)
{
    // TODO: maybe validate that this time is in the future?
    _aquire_scheduler_lock();
    px_current_task->state = TASK_SLEEPING;
    px_current_task->wakeup_time = time;
    _enqueue_sleeping(px_current_task);
    _schedule();
    _release_scheduler_lock();
}

void px_tasks_nano_sleep(uint64_t time)
{
    px_tasks_nano_sleep_until(_get_cpu_time_ns() + time);
}
