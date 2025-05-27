#ifndef CANVAS_H
#define CANVAS_H

#define CANVAS_WIDTH 40
#define CANVAS_HEIGHT 15

#include "mypthreads.h" 
#include "mutex.h"

typedef struct {
    char grid[CANVAS_HEIGHT][CANVAS_WIDTH];
    my_mutex_t lock;
} canvas_t;

void canvas_init(canvas_t *canvas);
void canvas_draw(canvas_t *canvas, int x, int y, char c);
void canvas_render(canvas_t *canvas);  
void canvas_clear(canvas_t *canvas);
void canvas_draw_and_render(canvas_t *canvas, int x, int y, char c);



#endif
