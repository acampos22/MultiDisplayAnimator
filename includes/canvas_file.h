#ifndef CANVAS_FILE_H
#define CANVAS_FILE_H

#include <pthread.h>

#define CANVAS_WIDTH 40
#define CANVAS_HEIGHT 15
#define CANVAS_FILENAME "canvas.txt"

extern pthread_mutex_t canvas_mutex[CANVAS_HEIGHT][CANVAS_WIDTH];

void canvas_file_init();
void canvas_file_clear();
void canvas_file_draw(int x, int y, char c);
void canvas_file_render();
int canvas_file_is_free(int x, int y);

#endif
