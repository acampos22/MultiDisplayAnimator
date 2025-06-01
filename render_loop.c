#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700

#include "../includes/canvas_file.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int ymin = 0, ymax = CANVAS_HEIGHT - 1;

    // Leer argumentos -ymin y -ymax
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-ymin") == 0 && i + 1 < argc) {
            ymin = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-ymax") == 0 && i + 1 < argc) {
            ymax = atoi(argv[++i]);
        }
    }

    canvas_file_init();

    while (1) {
        FILE *f = fopen(CANVAS_FILENAME, "r");
        if (!f) {
            perror("fopen");
            continue;
        }

        printf("\033[H");
        char line[CANVAS_WIDTH + 2];
        for (int i = 0; i < CANVAS_HEIGHT; i++) {
            if (!fgets(line, sizeof(line), f)) break;
            if (i >= ymin && i <= ymax) {
                printf("%s", line);
            }
        }
        fclose(f);
        usleep(100000);
    }

    return 0;
}
