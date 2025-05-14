// include/mypthreads.h
#ifndef MYPTHREADS_H
#define MYPTHREADS_H

#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <stdint.h>

typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} thread_state_t;

typedef enum {
    RR,
    LOTTERY,
    REALTIME
} scheduler_type_t;

#define STACK_SIZE 32768

typedef struct my_thread {
    int id;
    jmp_buf context;
    void *stack;
    thread_state_t state;
    void *retval;
    int detached;
    struct my_thread *next;

    // Scheduling info
    scheduler_type_t sched_type;
    int tickets; // Para LOTTERY
    int deadline; // Para REALTIME
} my_thread_t;

int my_thread_create(my_thread_t **thread, scheduler_type_t sched, void *(*start_routine)(void *), void *arg);
void my_thread_end(void *retval);
void my_thread_yield();
int my_thread_join(my_thread_t *thread, void **retval);
int my_thread_detach(my_thread_t *thread);
int my_thread_chsched(my_thread_t *thread, scheduler_type_t sched);

#endif
