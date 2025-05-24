#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700
#include "../includes/canvas_file.h"
#include <unistd.h>

int main() {
    canvas_file_init();
    while (1) {
        canvas_file_render();
        usleep(100000); // refresco cada 100ms
    }
    return 0;
}
