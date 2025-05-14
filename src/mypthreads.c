// src/mypthreads.c
#include "../includes/mypthreads.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static my_thread_t *current_thread = NULL;
static int next_id = 1;

int my_thread_create(my_thread_t **thread, scheduler_type_t sched, void *(*start_routine)(void *), void *arg) {
    *thread = malloc(sizeof(my_thread_t));
    if (!*thread) return -1;

    (*thread)->id = next_id++;
    (*thread)->state = READY;
    (*thread)->sched_type = sched;
    (*thread)->stack = malloc(STACK_SIZE);
    (*thread)->detached = 0;
    (*thread)->retval = NULL;

    if (!(*thread)->stack) return -1;

    if (setjmp((*thread)->context) == 0) {
        //stack puntero
        uintptr_t sp = (uintptr_t)(*thread)->stack + STACK_SIZE - sizeof(uintptr_t);
        memcpy((*thread)->context, (*thread)->context, sizeof(jmp_buf));
        ((void **)(*thread)->context)[0] = (void *)start_routine;

        return 0;
    } else {
        (*thread)->retval = start_routine(arg);
        my_thread_end((*thread)->retval);
        return 0;
    }
}

void my_thread_end(void *retval) {
    current_thread->retval = retval;
    current_thread->state = TERMINATED;

    // Yield al scheduler
    my_thread_yield();
}

void my_thread_yield() {
    printf("Yield ejecutado (implementar)\n");
}
