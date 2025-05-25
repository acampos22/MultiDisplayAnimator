#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700

#include "shape.h"
#include "../includes/utils.h"
#include "../includes/canvas_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../includes/shape.h"

#define HANDOFF_PREFIX "handoff_"

typedef struct {
    int id;
    int y_min;
    int y_max;
} region_t;

void wait_for_handoff(const char *target_id) {
    char filename[64];
    snprintf(filename, sizeof(filename), "%s%s", HANDOFF_PREFIX, target_id);

    while (access(filename, F_OK) != 0) {
        usleep(100000);
    }
    printf("[HANDOFF] Recibido control: %s\n", filename);
}

void signal_handoff(const char *target_id) {
    char filename[64];
    snprintf(filename, sizeof(filename), "%s%s", HANDOFF_PREFIX, target_id);

    FILE *f = fopen(filename, "w");
    if (f) {
        fprintf(f, "start");
        fclose(f);
        printf("[HANDOFF] Pasado control a: %s\n", filename);
    }
}

void run_script(char **script, int lines, char id, region_t region) {
    for (int i = 0; i < lines; i++) {
        char *line = script[i];
        printf("[MONITOR %c] Ejecutando: %s\n", id, line);

        if (strncmp(line, "draw_shape", 10) == 0) {
            int x, y, angle;
            char file[64];
        
            int parsed = sscanf(line, "draw_shape x=%d y=%d file=%s angle=%d", &x, &y, file, &angle);
            if (parsed != 4) {
                printf("[ERROR] Formato inválido en draw_shape: %s\n", line);
                continue;
            }
        
            Shape *s = load_shape(file);
            if (!s) {
                printf("[ERROR] No se pudo cargar forma desde archivo: %s\n", file);
                continue;
            }
        
            Shape *rotada = rotate_shape(s, angle);
            if (!rotada) {
                printf("[ERROR] No se pudo rotar la forma con ángulo %d\n", angle);
                free(s);
                continue;
            }
        
            // Limpieza previa si existe una figura anterior
            static Shape *prev_shape = NULL;
            static int prev_x = -1, prev_y = -1;
        
            if (prev_shape) {
                clear_shape_from_canvas(prev_x, prev_y, prev_shape);
                free(prev_shape);
                prev_shape = NULL;
            }
        
            draw_shape_on_canvas(x, y, rotada);
        
            // Guardar referencia a la actual para limpiarla luego
            prev_shape = rotada;
            prev_x = x;
            prev_y = y;
        
            free(s);
        }
        

        else if (strncmp(line, "draw", 4) == 0) {
            int x, y;
            sscanf(line, "draw x=%d y=%d char=%*c", &x, &y);
            if (y < region.y_min || y > region.y_max) continue;
            canvas_file_draw(x, y, id);
        }

        else if (strncmp(line, "move", 4) == 0) {
            int x, y, dx, dy, steps;
            sscanf(line, "move x=%d y=%d dx=%d dy=%d steps=%d char=%*c", &x, &y, &dx, &dy, &steps);

            for (int s = 0; s < steps; s++) {
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
        printf("[CLEANUP] Eliminado: %s\n", cleanup_file);
    }

    region_t region = { .id = id, .y_min = y_min, .y_max = y_max };

    int count;
    char **lines = read_script(script_file, &count);
    if (!lines) {
        printf("No se pudo leer el script\n");
        return 1;
    }

    run_script(lines, count, letter, region);
    return 0;
}
