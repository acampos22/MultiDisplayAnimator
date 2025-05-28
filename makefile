CC = gcc
CFLAGS = -Wall -g -D_XOPEN_SOURCE=700
INCLUDES = -Iincludes

SRC_COMMON = src/canvas_file.c src/utils.c src/shape.c

MONITOR_PROCESS_SRC = monitor_process.c
RENDER_LOOP_SRC = render_loop.c

MONITOR_PROCESS = monitor_process
RENDER_LOOP = render_loop

all: $(MONITOR_PROCESS) $(RENDER_LOOP)

$(MONITOR_PROCESS): $(MONITOR_PROCESS_SRC) $(SRC_COMMON)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(RENDER_LOOP): $(RENDER_LOOP_SRC) $(SRC_COMMON)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

clean:
	rm -f $(MONITOR_PROCESS) $(RENDER_LOOP)
