// monitor_process.c
#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700
#include "shape.h"
#include "../includes/utils.h"
#include "../includes/canvas_file.h"
#include "../includes/monitor_process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../includes/mypthreads.h"


#define HANDOFF_PREFIX "handoff_"

typedef struct {
    int id;
    int y_min;
    int y_max;
} region_t;

void debug_log(const char *msg) {
    fprintf(stderr, "[DEBUG] %s\n", msg);
}

long get_elapsed_time_ms(struct timeval start_time) {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec - start_time.tv_sec) * 1000L + (now.tv_usec - start_time.tv_usec) / 1000L;
}

void show_boom(int x, int y, const Shape *shape) {
    if (shape) clear_shape_from_canvas(x, y, shape);
    canvas_file_draw(x, y, '*');
    canvas_file_draw(x + 1, y, 'B');
    canvas_file_draw(x + 2, y, 'O');
    canvas_file_draw(x + 3, y, 'O');
    canvas_file_draw(x + 4, y, 'M');
    usleep(600000);
    for (int i = 0; i < 5; i++) canvas_file_draw(x + i, y, ' ');
}

void wait_for_handoff(const char *target_id) {
    char filename[64];
    snprintf(filename, sizeof(filename), "%s%s", HANDOFF_PREFIX, target_id);
    while (access(filename, F_OK) != 0) usleep(100000);
}

void signal_handoff(const char *target_id) {
    char filename[64];
    snprintf(filename, sizeof(filename), "%s%s", HANDOFF_PREFIX, target_id);
    FILE *f = fopen(filename, "w");
    if (f) {
        fprintf(f, "start");
        fclose(f);
    }
}

void run_script(char **script, int lines, char id, region_t region, struct timeval start_time) {
    int i = 0;
    printf("🟢 Entrando a run_script para hilo %d\n", region.id);
    Shape *prev_shape = NULL;
    int prev_x = -1, prev_y = -1;

    while (i < lines) {
        int lifetime_start = 0, lifetime_end = 9999999;
        while (i < lines && strncmp(script[i], "lifetime", 8) != 0) i++;
        if (i >= lines) break;

        sscanf(script[i], "lifetime start=%d end=%d", &lifetime_start, &lifetime_end);
        i++;

        int block_start = i;
        while (i < lines && strncmp(script[i], "lifetime", 8) != 0) i++;
        int block_end = i;

        while (get_elapsed_time_ms(start_time) < lifetime_start) {
            usleep(10000);
        }

        for (int j = block_start; j < block_end; j++) {
            char *line = script[j];
            printf("📜 Ejecutando línea: %s", script[j]);

            if (get_elapsed_time_ms(start_time) > lifetime_end) {
                if (prev_shape) {
                    show_boom(prev_x, prev_y, prev_shape);
                    free(prev_shape);
                    prev_shape = NULL;
                }
                break;
            }

            if (strncmp(line, "draw_shape", 10) == 0) {
                int x, y, angle;
                char file[64];
                if (sscanf(line, "draw_shape x=%d y=%d file=%s angle=%d", &x, &y, file, &angle) != 4) continue;

                Shape *s = load_shape(file);
                if (!s) {
                    fprintf(stderr, "[ERROR] No se pudo cargar la forma desde: %s\n", file);
                    continue;
                }

                Shape *rotada = rotate_shape(s, angle);
                if (!rotada) {
                    fprintf(stderr, "[ERROR] Rotación fallida para: %s\n", file);
                    free(s);
                    continue;
                }

                if (prev_shape) {
                    clear_shape_from_canvas(prev_x, prev_y, prev_shape);
                    free(prev_shape);
                    prev_shape = NULL;
                }

                draw_shape_on_canvas(x, y, rotada);
                prev_shape = rotada;
                prev_x = x;
                prev_y = y;
                free(s);
                usleep(300000);
            }

            else if (strncmp(line, "move", 4) == 0) {
                if (prev_shape == NULL) {
                    printf("⚠️  No hay forma previa cargada para mover\n");
                    continue;
                }
            
                printf("➡️  Ejecutando movimiento\n");
            
                int dx, dy, steps;
                if (sscanf(line, "move x=%*d y=%*d dx=%d dy=%d steps=%d char=%*c", &dx, &dy, &steps) != 3) {
                    fprintf(stderr, "[ERROR] Línea move malformada: %s\n", line);
                    continue;
                }
            
                int x = prev_x;
                int y = prev_y;
            
                for (int s = 0; s < steps; s++) {
                    if (get_elapsed_time_ms(start_time) > lifetime_end) {
                        show_boom(x, y, prev_shape);
                        free(prev_shape);
                        prev_shape = NULL;
                        return;
                    }
            
                    int next_x = x + dx;
                    int next_y = y + dy;
            
                    if (next_x < 0 || next_x + prev_shape->width > CANVAS_WIDTH ||
                        next_y < 0 || next_y + prev_shape->height > CANVAS_HEIGHT) {
                        fprintf(stderr, "[WARN] Movimiento fuera del canvas: (%d, %d)\n", next_x, next_y);
                        continue;
                    }
            
                    // ⏳ Esperar hasta que el nuevo espacio esté libre o se agote el tiempo
                    while (!can_draw_shape_ignore_self(next_x, next_y, prev_shape, x, y)) {
                        if (get_elapsed_time_ms(start_time) > lifetime_end) {
                            show_boom(x, y, prev_shape);
                            free(prev_shape);
                            prev_shape = NULL;
                            return;
                        }
                        printf("🟥 Celda ocupada, esperando... (%d, %d)\n", next_x, next_y);
                        usleep(50000);
                        my_thread_yield();
                    }
            
                    // ✅ Movimiento válido, limpia posición vieja y dibuja en nueva
                    clear_shape_from_canvas(x, y, prev_shape);
                    draw_shape_on_canvas(next_x, next_y, prev_shape);
                    x = next_x;
                    y = next_y;
                    prev_x = x;
                    prev_y = y;
            
                    usleep(300000);
                    my_thread_yield();
                }
            }
            
            else if (strncmp(line, "draw", 4) == 0) {
                int x, y;
                sscanf(line, "draw x=%d y=%d char=%*c", &x, &y);
                if (y >= region.y_min && y <= region.y_max)
                    canvas_file_draw(x, y, id);
            }

            else if (strncmp(line, "handoff", 7) == 0) {
                char target[32];
                sscanf(line, "handoff %s", target);
                signal_handoff(target);
            }

            else if (strncmp(line, "wait", 4) == 0) {
                char source[32];
                sscanf(line, "wait %s", source);
                wait_for_handoff(source);
            }
        }
    }
}

void *monitor_run_script(void *arg) {
    monitor_args_t *args = (monitor_args_t *)arg;  // 🔁 Mover esta línea arriba

    printf("✅ Entró a monitor_run_script | hilo %d | script: %s\n",
           args->monitor_id, args->script_file);
    fflush(stdout);

    if (args->monitor_id == 0) {
        canvas_file_init();  
    }
    int count;
    char **lines = read_script(args->script_file, &count);
    if (!lines) {
        fprintf(stderr, "[ERROR] No se pudo leer el script: %s\n", args->script_file);
        return NULL;
    }

    region_t region = { .id = args->monitor_id, .y_min = args->y_min, .y_max = args->y_max };

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    run_script(lines, count, 'A' + args->monitor_id, region, start_time);
    return NULL;
}
