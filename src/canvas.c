#include "../includes/canvas.h"
#include <stdio.h>

void canvas_init(canvas_t *canvas) {
    my_mutex_init(&canvas->lock);
    canvas_clear(canvas);
}

void canvas_clear(canvas_t *canvas) {
    for (int i = 0; i < CANVAS_HEIGHT; i++)
        for (int j = 0; j < CANVAS_WIDTH; j++)
            canvas->grid[i][j] = ' ';
}

void canvas_draw(canvas_t *canvas, int x, int y, char c) {
    if (x < 0 || x >= CANVAS_WIDTH || y < 0 || y >= CANVAS_HEIGHT) return;

    my_mutex_lock(&canvas->lock);
    canvas->grid[y][x] = c;
    my_mutex_unlock(&canvas->lock);
}

void canvas_render(canvas_t *canvas) {
    my_mutex_lock(&canvas->lock);
    printf("\033[H");  // Mueve cursor a la esquina superior
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            putchar(canvas->grid[i][j]);
        }
        putchar('\n');
    }
    fflush(stdout);
    my_mutex_unlock(&canvas->lock);
}

// ✅ NUEVA: dibuja y renderiza bajo un único lock
void canvas_draw_and_render(canvas_t *canvas, int x, int y, char c) {
    if (x < 0 || x >= CANVAS_WIDTH || y < 0 || y >= CANVAS_HEIGHT) return;

    my_mutex_lock(&canvas->lock);
    canvas->grid[y][x] = c;

    printf("\033[H");  // Limpia pantalla
    for (int i = 0; i < CANVAS_HEIGHT; i++) {
        for (int j = 0; j < CANVAS_WIDTH; j++) {
            putchar(canvas->grid[i][j]);
        }
        putchar('\n');
    }
    fflush(stdout);
    my_mutex_unlock(&canvas->lock);
}
