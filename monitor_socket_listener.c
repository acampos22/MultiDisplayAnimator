#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "../includes/utils.h"
#include "../includes/shape.h"
#include "../includes/canvas_file.h"

#define MAX_LINE_LEN 128

void *socket_listener(void *arg) {
    int port = *((int *)arg);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(1);
    }

    printf("🟢 Escuchando en puerto %d\n", port);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        char buffer[MAX_LINE_LEN];
        int bytes = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) {
            close(client_fd);
            continue;
        }
        buffer[bytes] = '\0';

        printf("📥 Recibido por socket: %s\n", buffer);

        int x, y, dx, dy, steps;
        char fig;
        if (sscanf(buffer, "move x=%d y=%d dx=%d dy=%d steps=%d char=%c",
                   &x, &y, &dx, &dy, &steps, &fig) == 6) {

            for (int i = 0; i < steps; i++) {
                // Verificar si sale de la región
                if (y + dy < 0 || y + dy >= CANVAS_HEIGHT) {

                    send_shape_to_monitor("127.0.0.1", port, x, y, dx, dy, steps - i, fig);
                    break;
                }
                canvas_file_draw(x, y, ' ');
                x += dx;
                y += dy;
                canvas_file_draw(x, y, fig);
                usleep(300000);
            }
        }

        close(client_fd);
    }

    close(server_fd);
    return NULL;
}

void send_shape_to_monitor(const char *host, int port, int x, int y, int dx, int dy, int steps, char fig) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return;

    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, host, &dest.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("connect");
        close(sockfd);
        return;
    }

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "move x=%d y=%d dx=%d dy=%d steps=%d char=%c\n", x, y, dx, dy, steps, fig);
    write(sockfd, buffer, strlen(buffer));
    close(sockfd);
}
