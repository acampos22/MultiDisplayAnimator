#include "../includes/canvas.h"
#include "../includes/monitor.h"
#include "../includes/mypthreads.h"
#include "../includes/scheduler.h"
#include "../includes/utils.h"
#include <unistd.h>
#include <stdio.h>

int main() {
    scheduler_init();

    canvas_t canvas;
    canvas_init(&canvas);

    int len1, len2;
    char **script1 = read_script("test/monitor1.script", &len1);
    char **script2 = read_script("test/monitor2.script", &len2);

    if (!script1 || !script2) {
        printf("Error cargando scripts\n");
        return 1;
    }

    monitor_t mon1 = { .id = 1, .script = script1, .script_len = len1, .canvas = &canvas };
    monitor_t mon2 = { .id = 2, .script = script2, .script_len = len2, .canvas = &canvas };

    my_thread_t *t1, *t2;
    my_thread_create(&t1, RR, monitor_run, &mon1);
    my_thread_create(&t2, RR, monitor_run, &mon2);

    current_thread = scheduler_next_thread();
    setcontext(&current_thread->context);
    return 0;
}
