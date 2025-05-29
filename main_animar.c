#include "../includes/mypthreads.h"
#include "../includes/utils.h"
#include "../includes/monitor_process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern my_thread_t *current_thread;  // Importa el hilo actual desde mypthreads

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <archivo.script> <monitor_id>\n", argv[0]);
        return 1;
    }

    const char *script = argv[1];
    int id = atoi(argv[2]);

    FILE *f = fopen(script, "r");
    if (!f) {
        fprintf(stderr, "❌ No se pudo abrir el archivo de script: %s\n", script);
        return 1;
    }
    fclose(f);

    monitor_args_t *args = malloc(sizeof(monitor_args_t));
    if (!args) {
        fprintf(stderr, "❌ No se pudo asignar memoria para monitor_args_t\n");
        return 1;
    }

    strncpy(args->script_file, script, PATH_MAX);
    args->script_file[PATH_MAX - 1] = '\0';

    args->monitor_id = id;
    args->y_min = 0;
    args->y_max = 14;

    my_thread_t *hilo = NULL;
    if (my_thread_create(&hilo, REALTIME, monitor_run_script, args) != 0) {
        fprintf(stderr, "❌ Error al crear el hilo\n");
        free(args);
        return 1;
    }

    // Simula que el main es un hilo válido para que join funcione correctamente
    my_thread_t main_thread;
    memset(&main_thread, 0, sizeof(my_thread_t));
    main_thread.id = 0;
    main_thread.state = RUNNING;
    current_thread = &main_thread;

    my_thread_join(hilo, NULL);

    return 0;
}
