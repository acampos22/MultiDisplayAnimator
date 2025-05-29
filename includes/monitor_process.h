#ifndef MONITOR_PROCESS_H
#define MONITOR_PROCESS_H

// Argumentos que recibe cada monitor
typedef struct {
    const char *script_file;
    int monitor_id;
    int y_min;
    int y_max;
} monitor_args_t;

// Función que ejecuta un monitor en un hilo
void *monitor_run_script(void *arg);

#endif // MONITOR_PROCESS_H