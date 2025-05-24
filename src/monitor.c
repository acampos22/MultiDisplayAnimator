#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700
#include "../includes/monitor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  // <-- necesario para usleep()

void *monitor_run(void *arg) {
    monitor_t *mon = (monitor_t *)arg;

    for (int i = 0; i < mon->script_len; i++) {
        char *line = mon->script[i];

        if (strncmp(line, "draw", 4) == 0) {
            int x, y;
            char c;
            sscanf(line, "draw x=%d y=%d char=%c", &x, &y, &c);
            canvas_draw(mon->canvas, x, y, c);
        }
        else if (strncmp(line, "move", 4) == 0) {
            int x, y, dx, dy, steps;
            char c;
            sscanf(line, "move x=%d y=%d dx=%d dy=%d steps=%d char=%c",
                   &x, &y, &dx, &dy, &steps, &c);

            for (int s = 0; s < steps; s++) {
                printf("[MONITOR %d] Paso %d: x=%d y=%d\n", mon->id, s, x, y);
                canvas_draw(mon->canvas, x, y, ' ');
                x += dx;
                y += dy;
                canvas_draw_and_render(mon->canvas, x, y, c);
                usleep(300000);
                my_thread_yield();
            }
        }
    }

    return NULL;
}
