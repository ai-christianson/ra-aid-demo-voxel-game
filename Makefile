# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces

# Detect operating system
UNAME = $(shell uname -s)

# Set platform-specific flags
ifeq ($(UNAME), Darwin) # macOS
    # Check if Homebrew is available
    BREW_EXISTS = $(shell command -v brew 2>/dev/null)
    ifdef BREW_EXISTS
        # Get raylib path from Homebrew
        RAYLIB_PATH = $(shell brew --prefix raylib 2>/dev/null)
        ifdef RAYLIB_PATH
            # Add include and library paths for Homebrew installation
            CFLAGS += -I$(RAYLIB_PATH)/include
            LDFLAGS = -L$(RAYLIB_PATH)/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
        else
            # Fallback if raylib is not installed via Homebrew
            LDFLAGS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
        endif
    else
        # Fallback if Homebrew is not installed
        LDFLAGS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    endif
else ifeq ($(UNAME), Linux) # Linux
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
else ifeq ($(OS), Windows_NT) # Windows with MinGW
    LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
else # Default fallback
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

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