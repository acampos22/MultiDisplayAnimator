#ifndef MONITOR_SOCKET_LISTENER_H
#define MONITOR_SOCKET_LISTENER_H

#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700

#include <pthread.h>

// Función que corre un hilo que escucha un socket en el puerto especificado.
void *socket_listener(void *arg);

// Envía una figura a otro monitor a través de un socket.
void send_shape_to_monitor(const char *host, int port, int x, int y, int dx, int dy, int steps, char fig);

#endif // MONITOR_SOCKET_LISTENER_H
