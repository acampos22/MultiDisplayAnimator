#ifndef MONITOR_PROCESS_H
#define MONITOR_PROCESS_H


#include <limits.h>

typedef struct {
    char script_file[PATH_MAX];  // ✅ Cadena fija segura
    int monitor_id;
    int y_min;
    int y_max;
    long deadline_ms;
} monitor_args_t;



// Función que ejecuta un monitor en un hilo
void *monitor_run_script(void *arg);

// Depuración segura de errores
void debug_log(const char *msg);

#endif // MONITOR_PROCESS_H
