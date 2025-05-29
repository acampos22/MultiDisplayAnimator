#include "../includes/mypthreads.h"
#include "../includes/utils.h"
#include "../includes/monitor_process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char argv[]) {
    if (argc < 3) {
        printf("Uso: %s <archivo.script> <monitor_id>\n", argv[0]);
        return 1;
    }

    const char *script = argv[1];
    int id = atoi(argv[2]);

    monitor_args_t *args = malloc(sizeof(monitor_args_t));
    args->script_file = strdup(script);
    args->monitor_id = id;
    args->y_min = 0;
    args->y_max = 14;

    my_thread_t hilo;
    my_thread_create(&hilo, REALTIME, monitor_run_script, args);
    my_thread_join(&hilo, NULL);

    return 0;
}