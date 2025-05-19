#define _XOPEN_SOURCE 700
#include "../includes/mypthreads.h"
#include "../includes/scheduler.h"
#include "../includes/mutex.h"
#include <stdio.h>

my_mutex_t mutex;

void *func(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 2; i++) {
        my_mutex_lock(&mutex);

        // Sección crítica
        printf(" Hilo %d ENTRA a sección crítica (iteración %d)\n", id, i);
        for (volatile int j = 0; j < 100000000; j++);
        printf(" Hilo %d SALE de sección crítica (iteración %d)\n", id, i);

        my_mutex_unlock(&mutex);

        my_thread_yield();
    }

    return NULL;
}

int main() {
    scheduler_init();
    my_mutex_init(&mutex);

    my_thread_t *t1, *t2, *t3, *t4;
    int a = 1, b = 2, c = 3, d = 4;

    my_thread_create(&t1, RR, func, &a);
    my_thread_create(&t2, RR, func, &b);
    my_thread_create(&t3, RR, func, &c);
    my_thread_create(&t4, RR, func, &d);

    current_thread = scheduler_next_thread();
    setcontext(&current_thread->context);

    return 0;
}
