    #define _DEFAULT_SOURCE
    #define _XOPEN_SOURCE 700

    #include "../includes/shape.h"
    #include "../includes/canvas_file.h"
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <unistd.h>
    #include <stdbool.h>

    Shape *load_shape(const char *filename) {
        FILE *file = fopen(filename, "r");
        if (!file) return NULL;

        Shape *shape = malloc(sizeof(Shape));
        if (!shape) return NULL;
        memset(shape, 0, sizeof(Shape));

        if (fgets(shape->name, sizeof(shape->name), file)) {
            shape->name[strcspn(shape->name, "\n")] = 0;
        }

        char line[MAX_SHAPE_WIDTH + 2];
        int row = 0;
        while (fgets(line, sizeof(line), file) && row < MAX_SHAPE_HEIGHT) {
            line[strcspn(line, "\n")] = '\0';
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
                for (int i = 0; i < orig->height; i++)
                    for (int j = 0; j < orig->width; j++)
                        rot->pixels[i][j] = orig->pixels[i][j];
                break;
            case 90:
                rot->width = orig->height;
                rot->height = orig->width;
                for (int i = 0; i < orig->height; i++)
                    for (int j = 0; j < orig->width; j++)
                        rot->pixels[j][orig->height - 1 - i] = orig->pixels[i][j];
                break;
            case 180:
                rot->width = orig->width;
                rot->height = orig->height;
                for (int i = 0; i < orig->height; i++)
                    for (int j = 0; j < orig->width; j++)
                        rot->pixels[orig->height - 1 - i][orig->width - 1 - j] = orig->pixels[i][j];
                break;
            case 270:
                rot->width = orig->height;
                rot->height = orig->width;
                for (int i = 0; i < orig->height; i++)
                    for (int j = 0; j < orig->width; j++)
                        rot->pixels[orig->width - 1 - j][i] = orig->pixels[i][j];
                break;
            default:
                free(rot);
                return NULL;
        }

        return rot;
    }

    void draw_shape_on_canvas(int x, int y, const Shape *shape) {
        // Esperar hasta que todas las celdas necesarias estén libres
        for (int i = 0; i < shape->height; i++) {
            for (int j = 0; j < shape->width; j++) {
                char c = shape->pixels[i][j];
                if (c != ' ' && c != '\0') {
                    int abs_x = x + j;
                    int abs_y = y + i;
                    while (!canvas_file_is_free(abs_x, abs_y)) {
                        usleep(30000);  // Espera si no está libre
                    }
                }
            }
        }

        // Dibujar
        for (int i = 0; i < shape->height; i++) {
            for (int j = 0; j < shape->width; j++) {
                char c = shape->pixels[i][j];
                if (c != ' ' && c != '\0') {
                    int abs_x = x + j;
                    int abs_y = y + i;
                    canvas_file_draw(abs_x, abs_y, c);
                }
            }
        }
    }

    void clear_shape_from_canvas(int x, int y, const Shape *shape) {
        for (int i = 0; i < shape->height; i++) {
            for (int j = 0; j < shape->width; j++) {
                char c = shape->pixels[i][j];
                if (c != ' ' && c != '\0') {
                    int abs_x = x + j;
                    int abs_y = y + i;
                    canvas_file_draw(abs_x, abs_y, ' ');
                }
            }
        }
    }

    bool can_draw_shape_ignore_self(int new_x, int new_y, const Shape *shape, int old_x, int old_y) {
        for (int i = 0; i < shape->height; i++) {
            for (int j = 0; j < shape->width; j++) {
                char c = shape->pixels[i][j];
                if (c != ' ' && c != '\0') {
                    int abs_new_x = new_x + j;
                    int abs_new_y = new_y + i;
    
                    // ⚠️ IMPORTANTE: no asumimos que (new_x+dx, new_y+dy) == old_x+dx
                    // En cambio, revisamos si esa celda estaba ocupada antes
    
                    // Si la celda también estaba ocupada en la posición anterior, ignoramos la colisión
                    int local_old_x = abs_new_x - old_x;
                    int local_old_y = abs_new_y - old_y;
    
                    if (local_old_x >= 0 && local_old_x < shape->width &&
                        local_old_y >= 0 && local_old_y < shape->height &&
                        shape->pixels[local_old_y][local_old_x] != ' ' &&
                        shape->pixels[local_old_y][local_old_x] != '\0') {
                        continue;  // Celda ya ocupada antes por esta misma figura
                    }
    
                    if (!canvas_file_is_free(abs_new_x, abs_new_y)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    
    

    bool can_draw_shape(int new_x, int new_y, const Shape *shape, int old_x, int old_y) {
        for (int i = 0; i < shape->height; i++) {
            for (int j = 0; j < shape->width; j++) {
                char c = shape->pixels[i][j];
                if (c != ' ' && c != '\0') {
                    int abs_new_x = new_x + j;
                    int abs_new_y = new_y + i;
                    int abs_old_x = old_x + j;
                    int abs_old_y = old_y + i;
    
                    // 🔁 Si la celda es la misma que la que ya ocupaba antes, ignorala
                    if (abs_new_x == abs_old_x && abs_new_y == abs_old_y) {
                        continue;
                    }
    
                    if (!canvas_file_is_free(abs_new_x, abs_new_y)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    
    

