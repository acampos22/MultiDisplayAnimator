#define _XOPEN_SOURCE 700
#include "../includes/mypthreads.h"
#include "../includes/scheduler.h"
#include <stdio.h>

void *rr_func(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 3; i++) {
        printf("RR Hilo %d ejecuta (iter %d)\n", id, i);
        my_thread_yield();
    }
    return NULL;
}

void *lottery_func(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 5; i++) {
        printf("LOTTERY Hilo %d ejecuta (iter %d)\n", id, i);
        my_thread_yield();
    }
    return NULL;
}

void *realtime_func(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < 4; i++) {
        printf("REALTIME Hilo %d ejecuta (iter %d)\n", id, i);
        my_thread_yield();
    }
    return NULL;
}

int main() {
    scheduler_init();

    my_thread_t *rr1, *lottery1, *lottery2, *realtime1;

    int id1 = 1, id2 = 2, id3 = 3, id4 = 4;

    // Crear hilos
    my_thread_create(&rr1, RR, rr_func, &id1);
    my_thread_create(&lottery1, LOTTERY, lottery_func, &id2);
    my_thread_create(&lottery2, LOTTERY, lottery_func, &id3);
    my_thread_create(&realtime1, REALTIME, realtime_func, &id4);

    // Configurar LOTTERY
    lottery1->tickets = 10; // más chances
    lottery2->tickets = 1;

    // Configurar REALTIME
    realtime1->deadline = 2;

    current_thread = scheduler_next_thread();
    setcontext(&current_thread->context);

    return 0;
}
