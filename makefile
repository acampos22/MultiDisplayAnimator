# Compilador y banderas
CC = gcc
CFLAGS = -Wall -g -D_XOPEN_SOURCE=700
INCLUDES = -Iincludes

# Fuentes comunes
UTILS_SRC = src/utils.c
CANVAS_FILE_SRC = src/canvas_file.c

# Archivos principales
MONITOR_PROCESS_SRC = monitor_process.c
RENDER_LOOP_SRC = render_loop.c

# Ejecutables
MONITOR_PROCESS = monitor_process
RENDER_LOOP = render_loop

# Regla por defecto
all: $(MONITOR_PROCESS) $(RENDER_LOOP)

# Compilar monitor_process
$(MONITOR_PROCESS): $(MONITOR_PROCESS_SRC) $(CANVAS_FILE_SRC) $(UTILS_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Compilar render_loop
$(RENDER_LOOP): $(RENDER_LOOP_SRC) $(CANVAS_FILE_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Limpiar binarios
clean:
	rm -f $(MONITOR_PROCESS) $(RENDER_LOOP)
