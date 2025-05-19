#define _XOPEN_SOURCE 700
#include "../includes/mypthreads.h"
#include "../includes/scheduler.h"
#include <stdio.h>
#include <unistd.h>

void *func(void *arg) {
    int id = *(int *)arg;
    printf("Hilo %d arrancó\n", id);
    for (int i = 0; i < 3; i++) {
        printf("Hilo %d, iteración %d\n", id, i);
        my_thread_yield();
    }
    printf("Hilo %d terminó\n", id);
    return (void*)(uintptr_t)(id * 10);
}

int main() {
    scheduler_init();

    my_thread_t *t1, *t2;
    int a = 1, b = 2;

    my_thread_create(&t1, RR, func, &a);
    my_thread_create(&t2, RR, func, &b);

    current_thread = scheduler_next_thread();
    setcontext(&current_thread->context);

    void *retval1;

    my_thread_join(t1, &retval1);
    printf("Join hilo 1, retval = %ld\n", (long)retval1);

    my_thread_detach(t2);

    return 0;
}
