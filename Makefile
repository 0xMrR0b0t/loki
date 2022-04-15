CFLAGS := -pthread
CC := gcc
OUT := ./bin/
object_files := ./src/server.h ./src/main.c

all: $(object_files)
	$(CC) $(CFLAGS) ./src/main.c -o $(OUT)loki_x86_64
	$(CC) $(CFLAGS) ./src/main.c -m32 -o $(OUT)loki_x86


