#include "../includes/mypthreads.h"
#include "../includes/utils.h"
#include "../includes/monitor_process.h"
#include "../includes/scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>  // 🕒 Necesario para gettimeofday

extern my_thread_t *current_thread;

#define MAX_MONITORES 10

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "-c") != 0) {
        fprintf(stderr, "Uso: %s -c <archivo_configuracion>\n", argv[0]);
        return 1;
    }

    const char *config_file = argv[2];
    FILE *f = fopen(config_file, "r");
    if (!f) {
        fprintf(stderr, "❌ No se pudo abrir el archivo de configuración: %s\n", config_file);
        return 1;
    }

    // 🕒 Capturar el tiempo global de inicio
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
        args->y_min = 0;
        args->y_max = 14;
        args->deadline_ms = deadline;
        args->start_time = global_start_time;  // ✅ Asignar tiempo global

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

    my_thread_t *next = scheduler_next_thread();
    if (next) {
        current_thread = next;
        setcontext(&next->context);
    }

    return 0;
}
