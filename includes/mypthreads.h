// include/mypthreads.h
#define _XOPEN_SOURCE 700
#include <ucontext.h>

#ifndef MYPTHREADS_H
#define MYPTHREADS_H

#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <stdint.h>
#include <sys/ucontext.h>

// Tamaño del stack para cada hilo (32 KB)
#define STACK_SIZE 32768

typedef enum {
    CREATED = 0,
    READY = 1,
    RUNNING = 2,
    BLOCKED = 3,
    TERMINATED = 4
} thread_state_t;

typedef enum {
    RR,
    LOTTERY,
    REALTIME
} scheduler_type_t;

#define STACK_SIZE 32768

typedef struct my_thread {
    int id;                     // ID único del hilo
    ucontext_t context;         // Contexto (PC, SP, etc.)
    void *stack;                // Puntero al stack
    thread_state_t state;       // Estado actual
    void *retval;               // Valor de retorno
    int detached;               // 1 si está detached, 0 si no
    int joined;
    struct my_thread *next;     // Enlace para cola circular

    // Datos de planificación
    scheduler_type_t sched_type;
    int tickets;                // Sorteo
    int deadline;               // Tiempo real
} my_thread_t;

int my_thread_create(my_thread_t **thread, scheduler_type_t sched, void *(*start_routine)(void *), void *arg);
void my_thread_end(void *retval);
void my_thread_yield();
int my_thread_join(my_thread_t *thread, void **retval);
int my_thread_detach(my_thread_t *thread);
int my_thread_chsched(my_thread_t *thread, scheduler_type_t sched);

#endif
