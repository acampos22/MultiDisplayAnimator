#ifndef MONITOR_H
#define MONITOR_H

#include "canvas.h"

typedef struct {
    int id;
    char **script;      // Arreglo de líneas de comandos
    int script_len;     // Cantidad de líneas
    canvas_t *canvas;   // Referencia al canvas global
} monitor_t;

void *monitor_run(void *arg);  // Función para el hilo

#endif
