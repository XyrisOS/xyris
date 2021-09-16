/**
 * @file tasks.cpp
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-29
 *
 * @copyright Copyright the Xyris Contributors (c) 2020
 *
 */
#include <arch/Arch.hpp>
#include <sys/tasks.hpp>
#include <mem/heap.hpp>
#include <lib/stdio.hpp>
#include <dev/serial/rs232.hpp>
#include <stdint.h>         // Data type definitions
#include <x86gprintrin.h>   // needed for __rdtsc

/* forward declarations */
static void _enqueue_task(struct tasklist *, task *);
static struct task *_dequeue_task(struct tasklist *);
static void _cleaner_task_impl(void);
static void _schedule(void);
extern "C" void _tasks_enqueue_ready(struct task *task);
void tasks_update_time();
void _wakeup(struct task *task);

/* macro to create a new named tasklist and associated helper functions */
#define NAMED_TASKLIST(name) \
    struct tasklist tasks_##name = { /* Zero */ }; \
    static inline void _enqueue_##name(struct task *task) { \
        _enqueue_task(&tasks_##name, task); } \
    static inline struct task *_dequeue_##name() { \
        return _dequeue_task(&tasks_##name); }

struct task *current_task = NULL;
static struct task _cleaner_task;
static struct task _first_task;

struct tasklist tasks_ready = { /* Zero */ };
NAMED_TASKLIST(sleeping);
NAMED_TASKLIST(stopped);

// map between task state and the list it is in
static struct tasklist *_state_lists[TASK_STATE_COUNT] = {
    [TASK_RUNNING] = NULL, // not in a list
    [TASK_READY] = &tasks_ready,
    [TASK_SLEEPING] = &tasks_sleeping,
    [TASK_BLOCKED] = NULL, // in a list specific to the blocking primitive
    [TASK_STOPPED] = &tasks_stopped,
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
    uint32_t curr_tick = timer_tick;
    uint64_t curr_rtsc = __rdtsc();
    while (timer_tick != curr_tick + 1) { }
    curr_rtsc = __rdtsc() - curr_rtsc;
    _instr_per_ns = curr_rtsc / 1000000;
    // will be inaccurate, but it's the best we can do in these circumstances
    if (_instr_per_ns == 0) _instr_per_ns = 1;
}

static inline uint64_t _get_cpu_time_ns()
{
    return (__rdtsc()) / _instr_per_ns;
}

static void _print_task(const struct task *task)
{
    RS232::printf("%s is %s\n", task->name, _state_names[task->state]);
}

#ifdef DEBUG
#define TASK_ACTION(action, task) do { RS232::printf("%s ", action); _print_task(task); } while(0)
#else
#define TASK_ACTION(action, task)
#endif

static void _print_tasklist(const char *name, const struct tasklist *list)
{
    struct task *task = list->head;
    RS232::printf("%s:\n", name);
    while (task != NULL) {
        _print_task(task);
        task = task->next;
    }
}

static void _print_tasklist(const struct task *task)
{
    const struct tasklist *list = _state_lists[task->state];
    const char *state_name = _state_names[task->state];
    if (list == NULL) {
        RS232::printf("no tasklist available for %s tasks.\n", state_name);
        return;
    }

    _print_tasklist(state_name, list);
}

static void _on_timer();

void tasks_init()
{
    // get a pointer to the first task's tcb
    struct task *this_task = &_first_task;
    // discover the CPU speed for accurate scheduling
    _discover_cpu_speed();
    *this_task = {
        // this will be filled in when we switch to another task for the first time
        .stack_top = 0,
        // this will be the same for kernel tasks
        .page_dir = Paging::getPageDirPhysAddr(),
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
    (void) tasks_new(_cleaner_task_impl, &_cleaner_task, TASK_PAUSED, "[cleaner]");
    _cleaner_task.state = TASK_PAUSED;
    // update the timer variables
    _last_time = _get_cpu_time_ns();
    _last_timer_time = _last_time;
    // enable time slices
    _time_slice_remaining = TIME_SLICE_SIZE;
    // this is the current task
    current_task = this_task;
    timer_register_callback(_on_timer);
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
    tasks_exit();
    // prevent undefined behavior from returning to a random address
    PANIC("Attempted to schedule a stopped task\n");
}

// emulate a stack push
// this makes it easier and more intuitive to set up stacks
static inline void _stack_push_word(void **stack_pointer, size_t value)
{
    // decrement the stack pointer
    *(uintptr_t*)stack_pointer -= sizeof(size_t);
    // place the new value at that new address
    **(size_t**)stack_pointer = value;
}

static void _enqueue_task(struct tasklist *list, task *task)
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

static struct task *_dequeue_task(struct tasklist *list)
{
    struct task *task;
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

static void _remove_task(struct tasklist *list, struct task *task, struct task *previous)
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

extern "C" void _tasks_enqueue_ready(struct task *task)
{
    _enqueue_task(&tasks_ready, task);
}

static struct task *_tasks_dequeue_ready()
{
    return _dequeue_task(&tasks_ready);
}

struct task *tasks_new(void (*entry)(void), struct task *storage, task_state state, const char *name)
{
    struct task *new_task = storage;
    if (storage == NULL) {
        // allocate memory for our task structure
        new_task = (struct task*)malloc(sizeof(struct task));
        // panic if the alloc fails (we have no fallback)
        if (new_task == NULL) {
            PANIC("Unable to allocate memory for new task struct.\n");
        }
    }
    // allocate a page for this stack (we might change this later)
    // TODO: Should more than one page be allocated / freed?
    uint8_t *stack = (uint8_t *)Paging::newPage(1);
    if (stack == NULL) PANIC("Unable to allocate memory for new task stack.\n");
    // remember, the stack grows up
    void *stack_pointer = stack + ARCH_PAGE_SIZE;
    // a null stack frame to make the panic screen happy
    _stack_push_word(&stack_pointer, 0);
    // the last thing to happen is the task stopping function
    _stack_push_word(&stack_pointer, (size_t)_task_stopping);
    // next entry is the main function to call (the start of the task)
    _stack_push_word(&stack_pointer, (size_t)entry);
    // when this task is started, the CPU will pop off this value which will become the new EIP
    // we push this function to allow some setup code to be run from within the context of the new task
    _stack_push_word(&stack_pointer, (size_t)_task_starting);
    // our task switching code is going to pop four values off of the stack before returning
    _stack_push_word(&stack_pointer, 0);
    _stack_push_word(&stack_pointer, 0);
    _stack_push_word(&stack_pointer, 0);
    _stack_push_word(&stack_pointer, 0);
    new_task->stack_top = (uintptr_t)stack_pointer;
    new_task->page_dir = Paging::getPageDirPhysAddr();
    new_task->next = NULL;
    new_task->state = state;
    new_task->time_used = 0;
    new_task->name = name;
    new_task->alloc = storage == NULL ? ALLOC_DYNAMIC : ALLOC_STATIC;
    if (state == TASK_READY) {
        _tasks_enqueue_ready(new_task);
    }
    TASK_ACTION("create task", new_task);
    return new_task;
}

void tasks_update_time()
{
    uint64_t current_time = _get_cpu_time_ns();
    uint64_t delta = current_time - _last_time;
    if (current_task == NULL) {
        _idle_time += delta;
    } else {
        current_task->time_used += delta;
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
    if (current_task == NULL) {
        // we are currently idling and will schedule at a later time
        return;
    }
    // get the next task
    struct task *task = _tasks_dequeue_ready();
    // don't need to do anything if there's nothing ready to run
    if (task == NULL) {
        if (current_task->state == TASK_RUNNING) {
            // still running the same task
            // but also reset the time slice counter
            _time_slice_remaining = TIME_SLICE_SIZE;
            return;
        }
        // disable time slices because there are no tasks available to run
        _time_slice_remaining = 0;
        // count the time that this task ran for
        tasks_update_time();
        /*** idle ***/
        // borrow this task to return to once we're not idle anymore
        struct task *borrowed = current_task;
        // set the current task to null to indicate an idle state
        current_task = NULL;
        _idle_start = _get_cpu_time_ns();
        do {
            // enable interrupts to process timer and other events
            asm ("sti");
            // immediately halt the CPU
            asm ("hlt");
            // disable interrupts to restore our lock
            asm ("cli");
            // check if there's a task ready to be run
        } while (task = _tasks_dequeue_ready(), task == NULL);
        // count the time we spent idling
        tasks_update_time();
        // reset the current task
        current_task = borrowed;
        _idle_start = _idle_start - _get_cpu_time_ns();
        _idle_time += _idle_start;
    } else {
        // just do time accounting once
        tasks_update_time();
    }
    // reset the time slice because a new task is being scheduled
    _time_slice_remaining = TIME_SLICE_SIZE;
    // reset the last "timer time" since the time slice was reset
    _last_timer_time = _get_cpu_time_ns();
    // switch to the task
    tasks_switch_to(task);
}

void tasks_schedule()
{
    // we must lock on all scheduling operations
    _aquire_scheduler_lock();
    // run the scheduler
    _schedule();
    // this will run when we switch back to the calling task
    _release_scheduler_lock();
}

uint64_t tasks_get_self_time()
{
    tasks_update_time();
    return current_task->time_used;
}

void tasks_block_current(task_state reason)
{
    _aquire_scheduler_lock();
    current_task->state = reason;
    TASK_ACTION("block", current_task);
    _schedule();
    _release_scheduler_lock();
}

void tasks_unblock(struct task *task)
{
    _aquire_scheduler_lock();
    task->state = TASK_READY;
    TASK_ACTION("unblock", task);
    _tasks_enqueue_ready(task);
    _release_scheduler_lock();
}

void _wakeup(struct task *task)
{
    task->state = TASK_READY;
    task->wakeup_time = (0ULL - 1);
    _tasks_enqueue_ready(task);
    TASK_ACTION("wakeup", task);
}

static void _on_timer()
{
    _aquire_scheduler_lock();

    struct task *pre = NULL;
    struct task *task = tasks_sleeping.head;
    struct task *next;
    bool need_schedule = false;
    uint64_t time = _get_cpu_time_ns();
    uint64_t time_delta;

    while (task != NULL) {
        next = task->next;
        if (time >= task->wakeup_time) {
            //RS232::printf("timer: waking sleeping task\n");
            _remove_task(&tasks_sleeping, task, pre);
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
            //RS232::printf("timer: time slice expired\n");
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

void tasks_nano_sleep_until(uint64_t time)
{
    // TODO: maybe validate that this time is in the future?
    _aquire_scheduler_lock();
    current_task->state = TASK_SLEEPING;
    current_task->wakeup_time = time;
    _enqueue_sleeping(current_task);
    TASK_ACTION("sleep", current_task);
    _schedule();
    _release_scheduler_lock();
}

void tasks_nano_sleep(uint64_t time)
{
    tasks_nano_sleep_until(_get_cpu_time_ns() + time);
}

void tasks_exit()
{
    // userspace cleanup can happen here
    RS232::printf("task \"%s\" (0x%08x) exiting\n", current_task->name, (uint32_t)current_task);

    _aquire_scheduler_lock();
    // all scheduling-specific operations must happen here
    _enqueue_stopped(current_task);

    // the ordering of these two should really be reversed
    // but the scheduler currently isn't very smart
    tasks_block_current(TASK_STOPPED);

    tasks_unblock(&_cleaner_task);

    _release_scheduler_lock();
}

static void _clean_stopped_task(struct task *task)
{
    // free the stack page
    uintptr_t page = Arch::Memory::pageAlign(task->stack_top);
    // TODO: Should more than one page be allocated / freed?
    Paging::freePage((void *)page, 1);
    // somehow determine if the task was dynamically allocated or not
    // just assume statically allocated tasks will never exit (bad idea)
    if (task->alloc == ALLOC_DYNAMIC) free(task);
}

static void _cleaner_task_impl()
{
    for (;;) {
        struct task *task;
        _aquire_scheduler_lock();

        while (tasks_stopped.head != NULL) {
            task = _dequeue_stopped();
            RS232::printf("cleaning up task %s (0x%08x)\n", task->name ? task->name : "N/A", (uint32_t)task);
            _clean_stopped_task(task);
        }

        // a schedule occuring at this point would be okay
        // it just needs to occur before the loop repeats
        tasks_block_current(TASK_PAUSED);

        _release_scheduler_lock();
    }
}

void tasks_sync_block(struct task_sync *ts)
{
    _aquire_scheduler_lock();
#ifdef DEBUG
    if (ts->dbg_name != NULL) {
        RS232::printf("blocking %s\n", ts->dbg_name);
    }
#endif
    // push the current task to the waiting queue
    _enqueue_task(&ts->waiting, current_task);
    // now block until the mutex is freed
    tasks_block_current(TASK_BLOCKED);
    _release_scheduler_lock();
}

void tasks_sync_unblock(struct task_sync *ts)
{
    _aquire_scheduler_lock();
#ifdef DEBUG
    if (ts->dbg_name != NULL) {
        RS232::printf("unblocking %s\n", ts->dbg_name);
    }
#endif
    // iterate all tasks that were blocked and unblock them
    struct task *task = ts->waiting.head;
    struct task *next = NULL;
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
