#define _XOPEN_SOURCE 700
#include <ucontext.h>
#include "../includes/scheduler.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>


// Declaración de funciones internas
static my_thread_t *select_lottery();
static my_thread_t *select_realtime();

my_thread_t *head = NULL;
my_thread_t *tail = NULL;
my_thread_t *current_thread = NULL;

void scheduler_init() {
    head = tail = current_thread = NULL;
    srand(time(NULL));
}


void debug_scheduler() {
    printf("🔍 Estado actual del scheduler:\n");
    my_thread_t *tmp = head;
    if (!tmp) {
        printf("  🚫 Lista vacía\n");
        return;
    }

    do {
        printf("  🧵 Hilo %d | Estado: %d | Tipo: %d | Deadline: %d\n",
               tmp->id, tmp->state, tmp->sched_type, tmp->deadline);
        tmp = tmp->next;
    } while (tmp && tmp != head);
}

void scheduler_add_thread(my_thread_t *thread) {
    thread->state = READY;
    printf("📥 Agregado al scheduler: hilo %d\n", thread->id);

    if (!head) {
        head = tail = thread;
        thread->next = head;
    } else {
        tail->next = thread;
        tail = thread;
        tail->next = head;
    }
}

void scheduler_remove_thread(my_thread_t *thread) {
    if (!head || !thread) return;

    my_thread_t *prev = tail;
    my_thread_t *cur = head;

    do {
        if (cur == thread) {
            if (cur == head) head = cur->next;
            if (cur == tail) tail = prev;
            if (cur == cur->next) {
                head = tail = NULL;
            } else {
                prev->next = cur->next;
            }
            break;
        }
        prev = cur;
        cur = cur->next;
    } while (cur != head);
}
my_thread_t *scheduler_next_thread() {
    my_thread_t *selected = NULL;

    // LOTTERY
    selected = select_lottery();
    if (selected) {
        printf("🎯 Scheduler eligió hilo %d (tipo LOTTERY)\n", selected->id);
        return selected;
    }

    // REALTIME
    selected = select_realtime();
    if (selected) {
        printf("🎯 Scheduler eligió hilo %d (tipo REALTIME)\n", selected->id);
        return selected;
    }

    // ROUND ROBIN
    if (!head) return NULL;

    my_thread_t *start = current_thread ? current_thread : head;
    my_thread_t *next = (current_thread && current_thread->next) ? current_thread->next : head;

    if (!next) return NULL;

    do {
        if (next->state == READY && next->sched_type == RR) {
            printf("🎯 Scheduler eligió hilo %d (tipo RR)\n", next->id);
            return next;
        }
        next = next->next;
    } while (next && next != start);

    return NULL;
}


static my_thread_t *select_lottery() {
    static int lottery_turns = 0;
    const int MAX_LOTTERY_TURNS = 3;

    if (lottery_turns >= MAX_LOTTERY_TURNS) {
        lottery_turns = 0;
        return NULL;  // 🔁 Cede el turno a otro tipo (como RR)
    }

    int total_tickets = 0;
    my_thread_t *tmp = head;

    if (!tmp) return NULL;

    do {
        if (tmp->state == READY && tmp->sched_type == LOTTERY)
            total_tickets += tmp->tickets;
        tmp = tmp->next;
    } while (tmp != head);

    if (total_tickets == 0) return NULL;

    int winner = rand() % total_tickets;
    tmp = head;

    do {
        if (tmp->state == READY && tmp->sched_type == LOTTERY) {
            if (winner < tmp->tickets) {
                lottery_turns++;  // 🎯 Gana un turno más
                return tmp;
            }
            winner -= tmp->tickets;
        }
        tmp = tmp->next;
    } while (tmp != head);

    return NULL;
}

static my_thread_t *select_realtime() {
    static int realtime_turns = 0;
    const int MAX_REALTIME_TURNS = 3;

    if (realtime_turns >= MAX_REALTIME_TURNS) {
        realtime_turns = 0;
        return NULL;  // 💤 Cede el paso a RR o Lottery
    }

    my_thread_t *selected = NULL;
    my_thread_t *tmp = head;

    if (!tmp) return NULL;

    do {
        if (tmp->state == READY && tmp->sched_type == REALTIME) {
            if (!selected || tmp->deadline < selected->deadline)
                selected = tmp;
        }
        tmp = tmp->next;
    } while (tmp != head);

    if (selected) realtime_turns++;
    return selected;
}
