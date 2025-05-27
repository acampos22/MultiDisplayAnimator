#include "../includes/canvas_file.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void canvas_file_init() {
    canvas_file_clear();
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

    int fd = open(CANVAS_FILENAME, O_RDWR);
    if (fd < 0) return;

    int pos = y * (CANVAS_WIDTH + 1) + x;  // +1 por el \n
    lseek(fd, pos, SEEK_SET);
    write(fd, &c, 1);
    close(fd);
}

void canvas_file_render() {
    FILE *f = fopen(CANVAS_FILENAME, "r");
    if (!f) return;

    printf("\033[H"); // mover cursor al inicio
    char line[CANVAS_WIDTH + 2];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
}

int canvas_file_is_free(int x, int y) {
    if (x < 0 || x >= CANVAS_WIDTH || y < 0 || y >= CANVAS_HEIGHT)
        return 0;

    FILE *f = fopen(CANVAS_FILENAME, "r");
    if (!f) return 0;

    int pos = y * (CANVAS_WIDTH + 1) + x;
    fseek(f, pos, SEEK_SET);
    char c = fgetc(f);
    fclose(f);

    return (c == ' ' || c == '\n');  
}
