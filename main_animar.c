#define _XOPEN_SOURCE 700
#include "../includes/mypthreads.h"
#include "../includes/utils.h"
#include "../includes/monitor_process.h"
#include "../includes/scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include "canvas.h"
#include "monitor_socket_listener.h"

extern my_thread_t *current_thread;

#define MAX_MONITORES 10

int main(int argc, char *argv[]) {
    const char *config_file = NULL;
    int port_actual = 0;
    int port_siguiente = 0;
    int ymin = 0, ymax = CANVAS_HEIGHT - 1;

    // Leer argumentos
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            config_file = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            port_actual = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            port_siguiente = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-ymin") == 0 && i + 1 < argc) {
            ymin = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-ymax") == 0 && i + 1 < argc) {
            ymax = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc)
            port_siguiente = atoi(argv[++i]);

    }

    if (!config_file) {
        fprintf(stderr, "Uso: %s -c <archivo_configuracion> [-p puerto] [-n siguiente] [-ymin v] [-ymax v]\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(config_file, "r");
    if (!f) {
        fprintf(stderr, "❌ No se pudo abrir el archivo de configuración: %s\n", config_file);
        return 1;
    }

    struct timeval global_start_time;
    gettimeofday(&global_start_time, NULL);

    my_thread_t *hilos[MAX_MONITORES];
    int num_hilos = 0;

    char line[256];
    while (fgets(line, sizeof(line), f) && num_hilos < MAX_MONITORES) {
        char script_path[PATH_MAX];
        int monitor_id;

        if (sscanf(line, "monitor %s %d", script_path, &monitor_id) != 2) {
            fprintf(stderr, "Línea inválida en config: %s", line);
            continue;
        }

        FILE *script = fopen(script_path, "r");
        if (!script) {
            fprintf(stderr, "❌ No se pudo abrir el script: %s\n", script_path);
            continue;
        }

        long deadline = 9999999;
        scheduler_type_t tipo = RR;

        char sline[128];
        while (fgets(sline, sizeof(sline), script)) {
            if (strncmp(sline, "type=RR", 7) == 0) tipo = RR;
            else if (strncmp(sline, "type=LOTTERY", 12) == 0) tipo = LOTTERY;
            else if (strncmp(sline, "type=REALTIME", 13) == 0) tipo = REALTIME;

            if (strncmp(sline, "lifetime", 8) == 0) {
                int start = 0, end = 9999999;
                if (sscanf(sline, "lifetime start=%d end=%d", &start, &end) == 2) {
                    deadline = end;
                }
                break;
            }
        }
        fclose(script);

        monitor_args_t *args = malloc(sizeof(monitor_args_t));
        strncpy(args->script_file, script_path, PATH_MAX);
        args->monitor_id = monitor_id;
        args->y_min = ymin;
        args->y_max = ymax;
        args->deadline_ms = deadline;
        args->start_time = global_start_time;
        args->port_siguiente = port_siguiente;

        if (my_thread_create(&hilos[num_hilos], tipo, monitor_run_script, args) != 0) {
            fprintf(stderr, "Error creando hilo %d\n", num_hilos);
            free(args);
            continue;
        }

        hilos[num_hilos]->deadline = args->deadline_ms;
        num_hilos++;
    }

    fclose(f);

    static my_thread_t main_thread;
    memset(&main_thread, 0, sizeof(my_thread_t));
    main_thread.id = 0;
    main_thread.state = RUNNING;
    current_thread = &main_thread;
    pthread_t socket_thread;
    pthread_create(&socket_thread, NULL, socket_listener, &port_actual);


    my_thread_t *next = scheduler_next_thread();
    if (next) {
        current_thread = next;
        setcontext(&next->context);
    }

    return 0;
}
