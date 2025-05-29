#include "../includes/canvas_file.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

pthread_mutex_t canvas_mutex[CANVAS_HEIGHT][CANVAS_WIDTH];

void canvas_file_init() {
    FILE *f = fopen(CANVAS_FILENAME, "w");
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            fputc(' ', f);
        }
        fputc('\n', f);
    }
    fclose(f);

    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            pthread_mutex_init(&canvas_mutex[i][j], NULL);
        }
    }
}

void canvas_file_clear() {
    FILE *f = fopen(CANVAS_FILENAME, "w");
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            fputc(' ', f);
        }
        fputc('\n', f);
    }
    fclose(f);
}

void canvas_file_draw(int x, int y, char c) {
    if (x < 0 || x >= CANVAS_WIDTH || y < 0 || y >= CANVAS_HEIGHT) return;

    pthread_mutex_lock(&canvas_mutex[y][x]);
    int fd = open(CANVAS_FILENAME, O_RDWR);
    if (fd < 0) {
        pthread_mutex_unlock(&canvas_mutex[y][x]);
        return;
    }

    int pos = y * (CANVAS_WIDTH + 1) + x;
    lseek(fd, pos, SEEK_SET);
    write(fd, &c, 1);
    close(fd);
    pthread_mutex_unlock(&canvas_mutex[y][x]);
}

int canvas_file_is_free(int x, int y) {
    if (x < 0 || x >= CANVAS_WIDTH || y < 0 || y >= CANVAS_HEIGHT)
        return 0;

    pthread_mutex_lock(&canvas_mutex[y][x]);

    FILE *f = fopen(CANVAS_FILENAME, "r");
    if (!f) {
        pthread_mutex_unlock(&canvas_mutex[y][x]);
        return 0;
    }
    int pos = y * (CANVAS_WIDTH + 1) + x;
    fseek(f, pos, SEEK_SET);
    char c = fgetc(f);
    fclose(f);
    pthread_mutex_unlock(&canvas_mutex[y][x]);
    if (c != ' ') {
        printf("🟥 Celda ocupada (%d,%d) contiene: '%c'\n", x, y, c);
    }    
    return (c == ' ' || c == '\n');    
}
void canvas_file_render() {
    FILE *f = fopen(CANVAS_FILENAME, "r");
    if (!f) return;

    printf("\033[H");
    char line[CANVAS_WIDTH + 2];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
}
