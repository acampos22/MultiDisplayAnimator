# Compilador y banderas
CC = gcc
CFLAGS = -Wall -g -D_XOPEN_SOURCE=700
INCLUDES = -Iincludes

# Fuentes comunes
SRC_COMMON = src/canvas_file.c src/utils.c src/shape.c

# Archivos fuente
MONITOR_PROCESS_SRC = monitor_process.c
RENDER_LOOP_SRC = render_loop.c

# Ejecutables
MONITOR_PROCESS = monitor_process
RENDER_LOOP = render_loop

# Regla por defecto
all: $(MONITOR_PROCESS) $(RENDER_LOOP)

# Compilar monitor_process
$(MONITOR_PROCESS): $(MONITOR_PROCESS_SRC) $(SRC_COMMON)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Compilar render_loop
$(RENDER_LOOP): $(RENDER_LOOP_SRC) src/canvas_file.c src/utils.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Limpiar binarios
clean:
	rm -f $(MONITOR_PROCESS) $(RENDER_LOOP)
