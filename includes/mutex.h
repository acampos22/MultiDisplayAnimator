#include "../includes/mypthreads.h"


#ifndef MY_MUTEX_H
#define MY_MUTEX_H



typedef struct my_mutex {
    int locked;                  // 0 = libre, 1 = bloqueado
    my_thread_t *owner;          // hilo que tiene el lock
    my_thread_t *wait_queue;     // lista de hilos bloqueados (cola simple)
} my_mutex_t;

// Funciones
int my_mutex_init(my_mutex_t *mutex);
int my_mutex_destroy(my_mutex_t *mutex);
int my_mutex_lock(my_mutex_t *mutex);
int my_mutex_trylock(my_mutex_t *mutex);
int my_mutex_unlock(my_mutex_t *mutex);

#endif
