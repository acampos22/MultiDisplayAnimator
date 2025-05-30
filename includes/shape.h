#ifndef SHAPE_H
#define SHAPE_H

#define MAX_SHAPE_WIDTH 16
#define MAX_SHAPE_HEIGHT 16

#include <stdbool.h>

typedef struct {
    char name[32];
    int width, height;
    char pixels[MAX_SHAPE_HEIGHT][MAX_SHAPE_WIDTH];
} Shape;

Shape *load_shape(const char *filename);
Shape *rotate_shape(const Shape *orig, int angle);
void draw_shape_on_canvas(int x, int y, const Shape *shape);  
void clear_shape_from_canvas(int x, int y, const Shape *shape);
bool can_draw_shape(int x, int y, const Shape *shape);

#endif
    