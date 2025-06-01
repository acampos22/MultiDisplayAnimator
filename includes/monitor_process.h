#ifndef MONITOR_PROCESS_H
#define MONITOR_PROCESS_H

#include <limits.h>
#include <sys/time.h>  // ✅ NECESARIO para struct timeval

typedef struct {
    char script_file[PATH_MAX];
    int monitor_id;
    int y_min;
    int y_max;
    int port_siguiente;
    long deadline_ms;
    struct timeval start_time;
} monitor_args_t;

// Función que ejecuta un monitor en un hilo
void *monitor_run_script(void *arg);

void debug_log(const char *msg);

#endif // MONITOR_PROCESS_H
