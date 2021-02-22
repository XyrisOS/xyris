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
#include <dev/serial/rs232.hpp>
#include <stdint.h>    // Data type definitions
// this is a literal hack to specifically circumvent validation checks
// it seems to work fine for the specific intrinsic we need
// we should probably investigate this further at some point...
#define _X86INTRIN_H_INCLUDED
#define __X86INTRIN_H
#include <ia32intrin.h> // needed for __rdtsc
#undef __X86INTRIN_H
#undef _X86INTRIN_H_INCLUDED

#include <arch/arch.hpp>

/* forward declarations */
static void _enqueue_task(px_tasklist_t *, px_task *);
static px_task_t *_dequeue_task(px_tasklist_t *);
static void _cleaner_task_impl(void);
static void _schedule(void);
extern "C" void _px_tasks_enqueue_ready(px_task_t *task);
static void _px_tasks_update_time();
static void _wakeup(px_task_t *task);

/* macro to create a new named tasklist and associated helper functions */
#define NAMED_TASKLIST(name) \
    px_tasklist_t px_tasks_##name = { /* Zero */ }; \
    static inline void _enqueue_##name(px_task_t *task) { \
        _enqueue_task(&px_tasks_##name, task); } \
    static inline px_task_t *_dequeue_##name() { \
        return _dequeue_task(&px_tasks_##name); }

px_task_t *px_current_task = NULL;
static px_task_t _cleaner_task;
static px_task_t _first_task;

px_tasklist_t px_tasks_ready = { /* Zero */ };
NAMED_TASKLIST(sleeping);
NAMED_TASKLIST(stopped);

// map between task state and the list it is in
static px_tasklist_t *_state_lists[TASK_STATE_COUNT] = {
    [TASK_RUNNING] = NULL, // not in a list
    [TASK_READY] = &px_tasks_ready,
    [TASK_SLEEPING] = &px_tasks_sleeping,
    [TASK_BLOCKED] = NULL, // in a list specific to the blocking primitive
    [TASK_STOPPED] = &px_tasks_stopped,
    [TASK_PAUSED] = NULL, // not in a list
};

// map between task state and its name
static const char *_state_names[TASK_STATE_COUNT] = {
    [TASK_RUNNING] = "RUNNING",
    [TASK_READY] = "READY",
    [TASK_SLEEPING] = "SLEEPING",
    [TASK_BLOCKED] = "BLOCKED",
    [TASK_STOPPED] = "STOPPED",
    [TASK_PAUSED] = "PAUSED",
};

static uint64_t _idle_time = 0;
static uint64_t _idle_start = 0;
static uint64_t _last_time = 0;
static uint64_t _time_slice_remaining = 0;
static uint64_t _last_timer_time = 0;
static size_t _scheduler_lock = 0;
static size_t _scheduler_postpone_count = 0;
static bool _scheduler_postponed = false;
static uint64_t _instr_per_ns;

static void _aquire_scheduler_lock()
{
    asm volatile("cli");
    _scheduler_postpone_count++;
    _scheduler_lock++;
}

static void _release_scheduler_lock()
{
    _scheduler_postpone_count--;
    if (_scheduler_postpone_count == 0) {
        if (_scheduler_postponed) {
            _scheduler_postponed = false;
            _schedule();
        }
    }
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

static void _print_task(const px_task_t *task)
{
    px_rs232_printf("%s is %s\n", task->name, _state_names[task->state]);
}

#ifdef DEBUG
#define TASK_ACTION(action, task) do { px_rs232_print(action " "); _print_task(task); } while(0)
#else
#define TASK_ACTION(action, task)
#endif

static void _print_tasklist(const char *name, const px_tasklist_t *list)
{
    px_task_t *task = list->head;
    px_rs232_printf("%s:\n", name);
    while (task != NULL) {
        _print_task(task);
        task = task->next;
    }
}

static void _print_tasklist(const px_task_t *task)
{
    const px_tasklist_t *list = _state_lists[task->state];
    const char *state_name = _state_names[task->state];
    if (list == NULL) {
        px_rs232_printf("no tasklist available for %s tasks.\n", state_name);
        return;
    }

    _print_tasklist(state_name, list);
}

static void _on_timer();

void px_tasks_init()
{
    // get a pointer to the first task's tcb
    px_task_t *this_task = &_first_task;
    // discover the CPU speed for accurate scheduling
    _discover_cpu_speed();
    *this_task = {
        // this will be filled in when we switch to another task for the first time
        .stack_top = 0,
        // this will be the same for kernel tasks
        .page_dir = px_get_phys_page_dir(),
        // this is a linked list with only this task 
        .next = NULL,
        // this task is currently running
        .state = TASK_RUNNING,
        // just say that this task hasn't spent any time running yet
        .time_used = 0,
        // Set wakeup time to 0 to make compiler happy
        .wakeup_time = 0,
        // name
        .name = "[main]",
        // this is not backed by dynamic memory
        .alloc = ALLOC_STATIC,
    };
    TASK_ACTION("create task", this_task);
    // create a task for the cleaner and set it's state to "paused"
    (void) px_tasks_new(_cleaner_task_impl, &_cleaner_task, TASK_PAUSED, "[cleaner]");
    _cleaner_task.state = TASK_PAUSED;
    // update the timer variables
    _last_time = _get_cpu_time_ns();
    _last_timer_time = _last_time;
    // enable time slices
    _time_slice_remaining = TIME_SLICE_SIZE;
    // this is the current task
    px_current_task = this_task;
    px_timer_register_callback(_on_timer);
}

static void _task_starting()
{
    // this is called whenever a new task is about to start
    // it is run in the context of the new task

    // the task before this caused the scheduler to lock
    // so we must unlock here
    _scheduler_lock--;
    if (_scheduler_lock == 0) {
        asm volatile("sti");
    }
}

static void _task_stopping()
{
    // this is called whenever a task is about to stop (i.e. it returned)
    // it is run in the context of the stopping task
    px_tasks_exit();
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
        list->tail->next = task;
    }
    // and now this task becomes the last task
    task->next = NULL;
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
    list->head = task->next;
    // so null its previous pointer
    if (list->head == NULL) {
        // if there are no more items in the list, then
        // the last item in the list will also be null
        list->tail = NULL;
    }
    // it doesn't make sense to have a next when it's not in a list
    task->next = NULL;
    return task;
}

static void _remove_task(px_tasklist_t *list, px_task_t *task, px_task_t *previous)
{
    // if this is true, something's not right...
    if (previous != NULL && previous->next != task) {
        PANIC("Bogus arguments to _remove_task.\n");
    }
    // update the head if necessary
    if (list->head == task) {
        list->head = task->next;
    }
    // update the tail if necessary
    if (list->tail == task) {
        list->tail = previous;
    }
    // update the previous task if necessary
    if (previous != NULL) {
        previous->next = task->next;
    }
    // it's not in any list anymore, so clear its next pointer
    task->next = NULL;
}

extern "C" void _px_tasks_enqueue_ready(px_task_t *task)
{
    _enqueue_task(&px_tasks_ready, task);
}

static px_task_t *_px_tasks_dequeue_ready()
{
    return _dequeue_task(&px_tasks_ready);
}

px_task_t *px_tasks_new(void (*entry)(void), px_task_t *storage, px_task_state state, const char *name)
{
    px_task_t *new_task = storage;
    if (storage == NULL) {
        // allocate memory for our task structure
        new_task = (px_task_t*)malloc(sizeof(px_task_t));
        // panic if the alloc fails (we have no fallback)
        if (new_task == NULL) {
            PANIC("Unable to allocate memory for new task struct.\n");
        }
    }
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
    new_task->stack_top = (uintptr_t)stack_pointer;
    new_task->page_dir = px_get_phys_page_dir();
    new_task->next = NULL;
    new_task->state = state;
    new_task->time_used = 0;
    new_task->name = name;
    new_task->alloc = storage == NULL ? ALLOC_DYNAMIC : ALLOC_STATIC;
    if (state == TASK_READY) {
        _px_tasks_enqueue_ready(new_task);
    }
    TASK_ACTION("create task", new_task);
    return new_task;
}

void _px_tasks_update_time()
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
    if (_scheduler_postpone_count != 0) {
        // don't schedule if there's more work to be done
        _scheduler_postponed = true;
        return;
    }
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
            // but also reset the time slice counter
            _time_slice_remaining = TIME_SLICE_SIZE;
            return;
        }
        // disable time slices because there are no tasks available to run
        _time_slice_remaining = 0;
        // count the time that this task ran for
        _px_tasks_update_time();
        /*** idle ***/
        // borrow this task to return to once we're not idle anymore
        px_task_t *borrowed = px_current_task;
        // set the current task to null to indicate an idle state
        px_current_task = NULL;
        _idle_start = _get_cpu_time_ns();
        do {
            // enable interrupts to process timer and other events
            asm ("sti");
            // immediately halt the CPU
            asm ("hlt");
            // disable interrupts to restore our lock
            asm ("cli");
            // check if there's a task ready to be run
        } while (task = _px_tasks_dequeue_ready(), task == NULL);
        // count the time we spent idling
        _px_tasks_update_time();
        // reset the current task
        px_current_task = borrowed;
        _idle_start = _idle_start - _get_cpu_time_ns();
        _idle_time += _idle_start;
    } else {
        // just do time accounting once
        _px_tasks_update_time();
    }
    // reset the time slice because a new task is being scheduled
    _time_slice_remaining = TIME_SLICE_SIZE;
#ifdef DEBUG
    px_rs232_print("switching to ");
    _print_task(task);
#endif
    // reset the last "timer time" since the time slice was reset
    _last_timer_time = _get_cpu_time_ns();
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
    _px_tasks_update_time();
    return px_current_task->time_used;
}

void px_tasks_block_current(px_task_state reason)
{
    _aquire_scheduler_lock();
    px_current_task->state = reason;
    TASK_ACTION("block", px_current_task);
    _schedule();
    _release_scheduler_lock();
}

void px_tasks_unblock(px_task_t *task)
{
    _aquire_scheduler_lock();
    task->state = TASK_READY;
    TASK_ACTION("unblock", task);
    _px_tasks_enqueue_ready(task);
    _release_scheduler_lock();
}

void _wakeup(px_task_t *task)
{
    task->state = TASK_READY;
    task->wakeup_time = (0ULL - 1);
    _px_tasks_enqueue_ready(task);
    TASK_ACTION("wakeup", task);
}

static void _on_timer()
{
    _aquire_scheduler_lock();

    px_task_t *pre = NULL;
    px_task_t *task = px_tasks_sleeping.head;
    px_task_t *next;
    bool need_schedule = false;
    uint64_t time = _get_cpu_time_ns();
    uint64_t time_delta;

    while (task != NULL) {
        next = task->next;
        if (time >= task->wakeup_time) {
            //px_rs232_print("timer: waking sleeping task\n");
            _remove_task(&px_tasks_sleeping, task, pre);
            _wakeup(task);
            task->next = NULL;
            need_schedule = true;
        } else {
            pre = task;
        }
        task = next;
    }

    if (_time_slice_remaining != 0) {
        time_delta = time - _last_timer_time;
        _last_timer_time = time;
        if (time_delta >= _time_slice_remaining) {
            // schedule (and maybe pre-empt)
            // the schedule function will reset the time slice
            //px_rs232_print("timer: time slice expired\n");
            need_schedule = true;
        } else {
            // decrement the time slice counter
            _time_slice_remaining -= time_delta;
        }
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
    TASK_ACTION("sleep", px_current_task);
    _schedule();
    _release_scheduler_lock();
}

void px_tasks_nano_sleep(uint64_t time)
{
    px_tasks_nano_sleep_until(_get_cpu_time_ns() + time);
}

void px_tasks_exit()
{
    // userspace cleanup can happen here
    px_rs232_printf("task \"%s\" (0x%08x) exiting\n", px_current_task->name, (uint32_t)px_current_task);

    _aquire_scheduler_lock();
    // all scheduling-specific operations must happen here
    _enqueue_stopped(px_current_task);

    // the ordering of these two should really be reversed
    // but the scheduler currently isn't very smart
    px_tasks_block_current(TASK_STOPPED);
    
    px_tasks_unblock(&_cleaner_task);

    _release_scheduler_lock();
}

static void _clean_stopped_task(px_task_t *task)
{
    // free the stack page
    uintptr_t page = task->stack_top & PAGE_ALIGN;
    px_free_page((void *)page, PAGE_SIZE - 1);
    // somehow determine if the task was dynamically allocated or not
    // just assume statically allocated tasks will never exit (bad idea)
    if (task->alloc == ALLOC_DYNAMIC) free(task);
}

static void _cleaner_task_impl()
{
    for (;;) {
        px_task_t *task;
        _aquire_scheduler_lock();
        
        while (px_tasks_stopped.head != NULL) {
            task = _dequeue_stopped();
            px_rs232_printf("cleaning up task %s (0x%08x)\n", task->name ? task->name : "N/A", (uint32_t)task);
            _clean_stopped_task(task);
        }

        // a schedule occuring at this point would be okay
        // it just needs to occur before the loop repeats
        px_tasks_block_current(TASK_PAUSED);

        _release_scheduler_lock();
    }
}

void px_tasks_sync_block(px_tasks_sync_t *ts)
{
    _aquire_scheduler_lock();
#ifdef DEBUG
    if (ts->dbg_name != NULL) {
        px_rs232_printf("blocking %s\n", ts->dbg_name);
    }
#endif
    // push the current task to the waiting queue
    _enqueue_task(&ts->waiting, px_current_task);
    // now block until the mutex is freed
    px_tasks_block_current(TASK_BLOCKED);
    _release_scheduler_lock();
}

void px_tasks_sync_unblock(px_tasks_sync_t *ts)
{
    _aquire_scheduler_lock();
#ifdef DEBUG
    if (ts->dbg_name != NULL) {
        px_rs232_printf("unblocking %s\n", ts->dbg_name);
    }
#endif
    // iterate all tasks that were blocked and unblock them
    px_task_t *task = ts->waiting.head;
    px_task_t *next = NULL;
    if (task == NULL) {
        // no other tasks were blocked
        goto exit;
    }
    do {
        next = task->next;
        _wakeup(task);
        task->next = NULL;
        task = next;
    } while (task != NULL);
    ts->waiting.head = NULL;
    ts->waiting.tail = NULL;
    // we woke up some tasks
    _schedule();
exit:
    _release_scheduler_lock();
}
