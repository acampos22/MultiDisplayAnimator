#include "../includes/canvas.h"
#include "../includes/monitor.h"
#include "../includes/mypthreads.h"
#include "../includes/scheduler.h"
#include <unistd.h>

int main() {
    scheduler_init();

    canvas_t canvas;
    canvas_init(&canvas);

    char *script1[] = {
        "draw x=5 y=3 char=A",
        "move x=5 y=3 dx=1 dy=0 steps=5 char=A"
    };

    char *script2[] = {
        "draw x=15 y=6 char=B",
        "move x=15 y=6 dx=-1 dy=0 steps=5 char=B"
    };

    monitor_t mon1 = { .id = 1, .script = script1, .script_len = 2, .canvas = &canvas };
    monitor_t mon2 = { .id = 2, .script = script2, .script_len = 2, .canvas = &canvas };

    my_thread_t *t1, *t2;
    my_thread_create(&t1, RR, monitor_run, &mon1);
    my_thread_create(&t2, RR, monitor_run, &mon2);

    current_thread = scheduler_next_thread();
    canvas_render(&canvas);  // render inicial
    setcontext(&current_thread->context);

    return 0;
}
