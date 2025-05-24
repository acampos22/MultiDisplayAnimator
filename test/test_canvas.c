#include "../includes/canvas.h"
#include <unistd.h>

int main() {
    canvas_t canvas;
    canvas_init(&canvas);

    canvas_draw(&canvas, 5, 2, '*');
    canvas_draw(&canvas, 10, 4, '@');
    canvas_draw(&canvas, 15, 6, '#');

    canvas_render(&canvas);

    sleep(2);
    canvas_clear(&canvas);
    canvas_render(&canvas);

    return 0;
}
