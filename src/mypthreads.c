#define _XOPEN_SOURCE 700
#include "../includes/mypthreads.h"
#include "../includes/scheduler.h"
#include <stdlib.h>
#include <stdio.h>

static int next_id = 1;

static void thread_starter(uintptr_t func_ptr, uintptr_t arg_ptr) {
    void *(*start_routine)(void *) = (void *(*)(void *))func_ptr;
    void *arg = (void *)arg_ptr;
    void *ret = start_routine(arg);
    my_thread_end(ret);
}

int my_thread_create(my_thread_t **thread, scheduler_type_t sched, void *(*start_routine)(void *), void *arg) {
    *thread = malloc(sizeof(my_thread_t));
    if (!*thread) return -1;

    (*thread)->id = next_id++;
    (*thread)->sched_type = sched;
    (*thread)->state = READY;
    (*thread)->retval = NULL;
    (*thread)->detached = 0;
    (*thread)->joined = 0;
    (*thread)->tickets = 1;
    (*thread)->deadline = 0;
    (*thread)->next = NULL;

    getcontext(&(*thread)->context);
    (*thread)->stack = malloc(STACK_SIZE);
    if (!(*thread)->stack) return -1;

    (*thread)->context.uc_stack.ss_sp = (*thread)->stack;
    (*thread)->context.uc_stack.ss_size = STACK_SIZE;
    (*thread)->context.uc_link = NULL;

    makecontext(&(*thread)->context, (void (*)())thread_starter, 2,
                (uintptr_t)start_routine, (uintptr_t)arg);

    scheduler_add_thread(*thread);
    return 0;
}

void my_thread_end(void *retval) {
    current_thread->retval = retval;
    current_thread->state = TERMINATED;

    scheduler_remove_thread(current_thread);

    if (current_thread->detached) {
        free(current_thread->stack);
        free(current_thread);
    }

    my_thread_t *next = scheduler_next_thread();

    if (!next) {
        // No hay más hilos vivos
        exit(0);
    }

    current_thread = next;
    setcontext(&current_thread->context);
}

void my_thread_yield() {
    my_thread_t *prev = current_thread;
    my_thread_t *next = scheduler_next_thread();

    if (!next || next == prev) return;

    current_thread = next;
    swapcontext(&prev->context, &next->context);
}

int my_thread_join(my_thread_t *thread, void **retval) {
    if (!thread || thread->detached) return -1;

    thread->joined = 1;

    while (thread->state != TERMINATED) {
        my_thread_yield();
    }

    if (retval)
        *retval = thread->retval;

    free(thread->stack);
    free(thread);

    return 0;
}

int my_thread_detach(my_thread_t *thread) {
    if (!thread || thread->detached) return -1;

    thread->detached = 1;

    if (thread->state == TERMINATED) {
        free(thread->stack);
        free(thread);
    }

    return 0;
}

int my_thread_chsched(my_thread_t *thread, scheduler_type_t sched) {
    if (!thread) return -1;
    thread->sched_type = sched;
    return 0;
}
