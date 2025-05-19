#define _XOPEN_SOURCE 700
#include <ucontext.h>


// include/scheduler.h
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "mypthreads.h"
extern my_thread_t *current_thread;

void scheduler_init();
void scheduler_add_thread(my_thread_t *thread);
void scheduler_remove_thread(my_thread_t *thread);
my_thread_t *scheduler_next_thread();

#endif
