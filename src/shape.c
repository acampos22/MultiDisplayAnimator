#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700
#include <unistd.h>
#include "../includes/shape.h"
#include "../includes/canvas_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Shape *load_shape(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    Shape *shape = malloc(sizeof(Shape));
    if (!shape) return NULL;

    memset(shape, 0, sizeof(Shape));

    // Leer nombre
    if (fgets(shape->name, sizeof(shape->name), file)) {
        shape->name[strcspn(shape->name, "\n")] = 0;
    }

    // Leer líneas de la forma
    char line[MAX_SHAPE_WIDTH + 2];
    int row = 0;
    while (fgets(line, sizeof(line), file) && row < MAX_SHAPE_HEIGHT) {
        line[strcspn(line, "\n")] = '\0';  // quitar \n
        int len = strlen(line);
        if (len > shape->width) shape->width = len;

        for (int col = 0; col < len && col < MAX_SHAPE_WIDTH; col++) {
            shape->pixels[row][col] = line[col];
        }
        row++;
    }

    shape->height = row;
    fclose(file);
    return shape;
}

Shape *rotate_shape(const Shape *orig, int angle) {
    Shape *rot = malloc(sizeof(Shape));
    if (!rot) return NULL;

    memset(rot->pixels, ' ', sizeof(rot->pixels));
    strcpy(rot->name, orig->name);

    switch (angle) {
        case 0:
            rot->width = orig->width;
            rot->height = orig->height;
            for (int i = 0; i < orig->height; i++) {
                for (int j = 0; j < orig->width; j++) {
                    rot->pixels[i][j] = orig->pixels[i][j];
                }
            }
            break;

        case 90:
            rot->width = orig->height;
            rot->height = orig->width;
            for (int i = 0; i < orig->height; i++) {
                for (int j = 0; j < orig->width; j++) {
                    rot->pixels[j][orig->height - 1 - i] = orig->pixels[i][j];
                }
            }
            break;

        case 180:
            rot->width = orig->width;
            rot->height = orig->height;
            for (int i = 0; i < orig->height; i++) {
                for (int j = 0; j < orig->width; j++) {
                    rot->pixels[orig->height - 1 - i][orig->width - 1 - j] = orig->pixels[i][j];
                }
            }
            break;

        case 270:
            rot->width = orig->height;
            rot->height = orig->width;
            for (int i = 0; i < orig->height; i++) {
                for (int j = 0; j < orig->width; j++) {
                    rot->pixels[orig->width - 1 - j][i] = orig->pixels[i][j];
                }
            }
            break;

        default:
            free(rot);
            return NULL;
    }

    return rot;
}


void draw_shape_on_canvas(int x, int y, const Shape *shape) {
    for (int i = 0; i < shape->height; i++) {
        for (int j = 0; j < shape->width; j++) {
            char c = shape->pixels[i][j];
            if (c != ' ' && c != '\0') {
                while (!canvas_file_is_free(x + j, y + i)) {
                    usleep(100000); 
                }
                canvas_file_draw(x + j, y + i, c);
            }
        }
    }
}
void clear_shape_from_canvas(int x, int y, const Shape *shape) {
    for (int i = 0; i < shape->height; i++) {
        for (int j = 0; j < shape->width; j++) {
            if (shape->pixels[i][j] != ' ' && shape->pixels[i][j] != '\0') {
                canvas_file_draw(x + j, y + i, ' ');
            }
        }
    }
}
