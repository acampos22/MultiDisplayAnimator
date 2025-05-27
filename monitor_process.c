#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700

#include "shape.h"
#include "../includes/utils.h"
#include "../includes/canvas_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define HANDOFF_PREFIX "handoff_"

typedef struct {
    int id;
    int y_min;
    int y_max;
} region_t;

long get_elapsed_time_ms(struct timeval start_time) {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec - start_time.tv_sec) * 1000L + (now.tv_usec - start_time.tv_usec) / 1000L;
}

void show_boom(int x, int y, const Shape *shape) {
    if (shape) {
        clear_shape_from_canvas(x, y, shape);
    }

    canvas_file_draw(x, y, '*');
    canvas_file_draw(x + 1, y, 'B');
    canvas_file_draw(x + 2, y, 'O');
    canvas_file_draw(x + 3, y, 'O');
    canvas_file_draw(x + 4, y, 'M');

    usleep(600000); 
    for (int i = 0; i < 5; i++) {
        canvas_file_draw(x + i, y, ' ');
    }
}

void wait_for_handoff(const char *target_id) {
    char filename[64];
    snprintf(filename, sizeof(filename), "%s%s", HANDOFF_PREFIX, target_id);
    while (access(filename, F_OK) != 0) {
        usleep(100000);
    }
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
    while (i < lines) {
        // Buscar el próximo bloque lifetime
        if (strncmp(script[i], "lifetime", 8) == 0) {
            int lifetime_start = 0, lifetime_end = 9999999;
            sscanf(script[i], "lifetime start=%d end=%d", &lifetime_start, &lifetime_end);
            i++;

            Shape *prev_shape = NULL;
            int prev_x = -1, prev_y = -1;

            // Esperar a que empiece el bloque
            while (get_elapsed_time_ms(start_time) < lifetime_start) {
                usleep(100000);
            }

            // Ejecutar comandos del bloque hasta que termine o llegue otro lifetime
            while (i < lines && strncmp(script[i], "lifetime", 8) != 0) {
                char *line = script[i];

                // Verificar si se pasó del tiempo permitido
                if (get_elapsed_time_ms(start_time) > lifetime_end) {
                    show_boom(prev_x, prev_y, prev_shape);
                    if (prev_shape) {
                        free(prev_shape);
                        prev_shape = NULL;
                    }
                    break;
                }

                if (strncmp(line, "draw_shape", 10) == 0) {
                    int x, y, angle;
                    char file[64];
                    if (sscanf(line, "draw_shape x=%d y=%d file=%s angle=%d", &x, &y, file, &angle) != 4) {
                        i++;
                        continue;
                    }

                    Shape *s = load_shape(file);
                    if (!s) {
                        i++;
                        continue;
                    }

                    Shape *rotada = rotate_shape(s, angle);
                    if (!rotada) {
                        free(s);
                        i++;
                        continue;
                    }

                    if (prev_shape) {
                        clear_shape_from_canvas(prev_x, prev_y, prev_shape);
                        free(prev_shape);
                    }

                    draw_shape_on_canvas(x, y, rotada);
                    prev_shape = rotada;
                    prev_x = x;
                    prev_y = y;

                    free(s);
                    usleep(400000);
                }

                else if (strncmp(line, "draw", 4) == 0) {
                    int x, y;
                    sscanf(line, "draw x=%d y=%d char=%*c", &x, &y);
                    if (y >= region.y_min && y <= region.y_max)
                        canvas_file_draw(x, y, id);
                }

                else if (strncmp(line, "move", 4) == 0) {
                    int x, y, dx, dy, steps;
                    sscanf(line, "move x=%d y=%d dx=%d dy=%d steps=%d char=%*c", &x, &y, &dx, &dy, &steps);
                    for (int s = 0; s < steps; s++) {
                        if (get_elapsed_time_ms(start_time) > lifetime_end) {
                            show_boom(prev_x, prev_y, prev_shape);
                            if (prev_shape) {
                                free(prev_shape);
                                prev_shape = NULL;
                            }
                            return;
                        }

                        if (y >= region.y_min && y <= region.y_max)
                            canvas_file_draw(x, y, ' ');
                        x += dx;
                        y += dy;
                        if (y >= region.y_min && y <= region.y_max)
                            canvas_file_draw(x, y, id);
                        usleep(300000);
                    }
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

                i++;
            }
        } else {
            i++; 
        }
    }
}

int main(int argc, char *argv[]) {
    canvas_file_init();

    if (argc != 6 && argc != 7) {
        printf("Uso: %s <archivo.script> <letra> <y_min> <y_max> <monitor_id> [--clean]\n", argv[0]);
        return 1;
    }

    const char *script_file = argv[1];
    char letter = argv[2][0];
    int y_min = atoi(argv[3]);
    int y_max = atoi(argv[4]);
    int id = atoi(argv[5]);

    if (argc == 7 && strcmp(argv[6], "--clean") == 0) {
        char cleanup_file[64];
        snprintf(cleanup_file, sizeof(cleanup_file), "handoff_monitor%d", id);
        remove(cleanup_file);
    }

    region_t region = { .id = id, .y_min = y_min, .y_max = y_max };

    int count;
    char **lines = read_script(script_file, &count);
    if (!lines) {
        printf("No se pudo leer el script\n");
        return 1;
    }

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    run_script(lines, count, letter, region, start_time);
    return 0;
}
