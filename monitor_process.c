#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700

#include "../includes/utils.h"
#include "../includes/canvas_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int id;
    int y_min;
    int y_max;
} region_t;

void run_script(char **script, int lines, char id, region_t region) {
    for (int i = 0; i < lines; i++) {
        char *line = script[i];
        printf("[MONITOR %c] Ejecutando línea: %s\n", id, line);

        if (strncmp(line, "draw", 4) == 0) {
            int x, y;
            sscanf(line, "draw x=%d y=%d char=%*c", &x, &y);
            if (y < region.y_min || y > region.y_max) continue;
            printf("[MONITOR %c] Dibujando en %d,%d\n", id, x, y);
            canvas_file_draw(x, y, id);
        }
        else if (strncmp(line, "move", 4) == 0) {
            int x, y, dx, dy, steps;
            sscanf(line, "move x=%d y=%d dx=%d dy=%d steps=%d char=%*c",
                   &x, &y, &dx, &dy, &steps);

            for (int s = 0; s < steps; s++) {
                if (y >= region.y_min && y <= region.y_max) {
                    canvas_file_draw(x, y, ' ');  // solo borra si es su zona
                }
                x += dx;
                y += dy;

                if (y >= region.y_min && y <= region.y_max) {
                    printf("[MONITOR %c] Dibujando en %d,%d\n", id, x, y);
                    canvas_file_draw(x, y, id);
                }
                else {
                    printf("[MONITOR %c] Fuera de zona en %d,%d, pausa\n", id, x, y);
                }
                usleep(300000);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    canvas_file_init();  // asegura que el canvas exista

    if (argc != 6) {
        printf("Uso: %s <archivo.script> <letra> <y_min> <y_max> <monitor_id>\n", argv[0]);
        return 1;
    }

    const char *script_file = argv[1];
    char letter = argv[2][0];
    int y_min = atoi(argv[3]);
    int y_max = atoi(argv[4]);
    int id = atoi(argv[5]);

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
