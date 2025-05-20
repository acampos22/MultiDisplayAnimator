# Compilador y banderas
CC = gcc
CFLAGS = -Wall -g -D_XOPEN_SOURCE=700
INCLUDES = -Iincludes

# Archivos fuente comunes
SRC = src/mypthreads.c src/scheduler.c src/mutex.c

# Archivos de prueba
DEMO_TEST = test/demo.c
DEMO_OUT = test/demo

MUTEX_TEST = test/mutex_demo.c
MUTEX_OUT = test/mutex_test

SCHEDULER_TEST = test/test_schedulers.c
SCHEDULER_OUT = test/test_schedulers
# Regla por defecto
all: $(DEMO_OUT) $(MUTEX_OUT) $(SCHEDULER_OUT)

# Compilar demo original
$(DEMO_OUT): $(SRC) $(DEMO_TEST)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) $(DEMO_TEST) -o $(DEMO_OUT)

# Compilar prueba de mutex
$(MUTEX_OUT): $(SRC) $(MUTEX_TEST)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) $(MUTEX_TEST) -o $(MUTEX_OUT)

# Compilar la prueba de schedulers
$(SCHEDULER_OUT): $(SRC) $(SCHEDULER_TEST)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) $(SCHEDULER_TEST) -o $(SCHEDULER_OUT)

# Limpiar binarios
clean:
	rm -f $(DEMO_OUT) $(MUTEX_OUT) $(SCHEDULER_OUT)
