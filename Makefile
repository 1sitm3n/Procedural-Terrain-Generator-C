# Makefile for Procedural Terrain Generator (C + OpenMP via clang+libomp on macOS)

CC      = /usr/bin/clang
CFLAGS  = -O3 -std=c11 -Wall -Wextra -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include
LDFLAGS = -L/opt/homebrew/opt/libomp/lib -lomp


SRCS = main.c terrain.c noise.c io.c
OBJS = $(SRCS:.c=.o)

TARGET = terrain_generator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
