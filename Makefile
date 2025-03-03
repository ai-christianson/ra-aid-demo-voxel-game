# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Source files and output
SOURCES = main.c voxel.c terrain.c player.c
EXECUTABLE = voxel_game

# Build targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(EXECUTABLE)

.PHONY: all clean