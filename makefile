CC = gcc
CFLAGS = -Wall -g -D_XOPEN_SOURCE=700
INCLUDES = -Iincludes

SRC_COMMON = src/canvas_file.c src/utils.c src/shape.c src/mypthreads.c src/scheduler.c src/mutex.c src/canvas.c
RENDER_LOOP_SRC = render_loop.c
MAIN_ANIMAR_SRC = main_animar.c
MONITOR_SRC = monitor_process.c monitor_socket_listener.c

RENDER_LOOP = render_loop
MAIN_ANIMAR = animar

all: $(RENDER_LOOP) $(MAIN_ANIMAR)

$(RENDER_LOOP): $(RENDER_LOOP_SRC) $(SRC_COMMON)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(MAIN_ANIMAR): $(MAIN_ANIMAR_SRC) $(MONITOR_SRC) $(SRC_COMMON)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

clean:
	rm -f $(RENDER_LOOP) $(MAIN_ANIMAR)
