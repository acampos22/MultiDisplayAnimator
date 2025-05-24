# Compilador y banderas
CC = gcc
CFLAGS = -Wall -g -D_XOPEN_SOURCE=700
INCLUDES = -Iincludes

# Archivos fuente comunes
SRC = src/mypthreads.c src/scheduler.c src/mutex.c

# Canvas y monitor
CANVAS_SRC = src/canvas.c
MONITOR_SRC = src/monitor.c

# Pruebas
DEMO_TEST = test/demo.c
DEMO_OUT = test/demo

MUTEX_TEST = test/mutex_demo.c
MUTEX_OUT = test/mutex_test

SCHEDULER_TEST = test/test_schedulers.c
SCHEDULER_OUT = test/test_schedulers

CANVAS_TEST = test/test_canvas.c
CANVAS_OUT = test/test_canvas

MONITOR_TEST = test/test_monitor.c
MONITOR_OUT = test/test_monitors

# Regla por defecto: compila las pruebas principales
all: $(DEMO_OUT) $(MUTEX_OUT) $(SCHEDULER_OUT)

# Compilar demo
$(DEMO_OUT): $(SRC) $(DEMO_TEST)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) $(DEMO_TEST) -o $(DEMO_OUT)

# Compilar prueba de mutex
$(MUTEX_OUT): $(SRC) $(MUTEX_TEST)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) $(MUTEX_TEST) -o $(MUTEX_OUT)

# Compilar prueba de schedulers
$(SCHEDULER_OUT): $(SRC) $(SCHEDULER_TEST)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) $(SCHEDULER_TEST) -o $(SCHEDULER_OUT)

# Compilar prueba del canvas
test_canvas: $(SRC) $(CANVAS_SRC) $(CANVAS_TEST)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(CANVAS_OUT)

# Compilar prueba de monitores
test_monitors: $(SRC) $(CANVAS_SRC) $(MONITOR_SRC) $(MONITOR_TEST)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(MONITOR_OUT)

# Limpiar binarios
clean:
	rm -f $(DEMO_OUT) $(MUTEX_OUT) $(SCHEDULER_OUT) $(CANVAS_OUT) $(MONITOR_OUT)
