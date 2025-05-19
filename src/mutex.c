#include "../includes/mutex.h"
#include "../includes/scheduler.h"
#include <stdlib.h>


int my_mutex_init(my_mutex_t *mutex) {
    if (!mutex) return -1;
    mutex->locked = 0;
    mutex->owner = NULL;
    mutex->wait_queue = NULL;
    return 0;
}

int my_mutex_destroy(my_mutex_t *mutex) {
    if (!mutex || mutex->locked) return -1; // No puedes destruir si está bloqueado
    return 0;
}

int my_mutex_trylock(my_mutex_t *mutex) {
    if (!mutex) return -1;
    if (mutex->locked) return -1;

    mutex->locked = 1;
    mutex->owner = current_thread;
    return 0;
}

int my_mutex_lock(my_mutex_t *mutex) {
    if (!mutex) return -1;

    if (!mutex->locked) {
        mutex->locked = 1;
        mutex->owner = current_thread;
        return 0;
    }

    // Mutex ya está bloqueado → hilo entra en espera
    current_thread->state = BLOCKED;

    // Insertar hilo al final de la cola de espera
    my_thread_t *temp = mutex->wait_queue;
    if (!temp) {
        mutex->wait_queue = current_thread;
    } else {
        while (temp->next) temp = temp->next;
        temp->next = current_thread;
    }

    my_thread_yield();  // Cede el CPU mientras espera
    return my_mutex_lock(mutex);  // Al volver, intenta de nuevo
}

int my_mutex_unlock(my_mutex_t *mutex) {
    if (!mutex || !mutex->locked || mutex->owner != current_thread)
        return -1;

    mutex->locked = 0;
    mutex->owner = NULL;

    // Despertar un hilo bloqueado si existe
    if (mutex->wait_queue) {
        my_thread_t *next = mutex->wait_queue;
        mutex->wait_queue = next->next;
        next->next = NULL;
        next->state = READY;
        scheduler_add_thread(next);
    }

    return 0;
}
