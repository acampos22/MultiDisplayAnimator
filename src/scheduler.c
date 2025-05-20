#define _XOPEN_SOURCE 700
#include <ucontext.h>
#include "../includes/scheduler.h"
#include <stdlib.h>
#include <time.h>

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

void scheduler_add_thread(my_thread_t *thread) {
    thread->state = READY;
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
    if (selected) return selected;

    // REALTIME
    selected = select_realtime();
    if (selected) return selected;

    // ROUND ROBIN
    if (!head) return NULL;

    my_thread_t *start = current_thread ? current_thread : head;
    my_thread_t *next = current_thread ? current_thread->next : head;

    do {
        if (next->state == READY && next->sched_type == RR)
            return next;
        next = next->next;
    } while (next != start);

    return NULL;
}

// LOTTERY Scheduler
static my_thread_t *select_lottery() {
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
            if (winner < tmp->tickets)
                return tmp;
            winner -= tmp->tickets;
        }
        tmp = tmp->next;
    } while (tmp != head);

    return NULL;
}

// REALTIME Scheduler
static my_thread_t *select_realtime() {
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

    return selected;
}
