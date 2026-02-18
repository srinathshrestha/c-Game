# ============================================================
# Makefile for 2D Car Racing Game
# Uses: SDL2, SDL2_image, SDL2_ttf
# Install deps on macOS: brew install sdl2 sdl2_image sdl2_ttf
# ============================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# SDL2 flags (auto-detected via sdl2-config)
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lm

# Source files (all .c files in src/)
SRC = $(wildcard src/*.c)

# Output binary name
TARGET = car_game

# ---- Build Rules ----

# Default: build the game
all: $(TARGET)

# Link all source files into the final binary
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -o $(TARGET) $(SRC) $(SDL_LIBS)

# Remove the compiled binary
clean:
	rm -f $(TARGET)

# Rebuild from scratch
rebuild: clean all

.PHONY: all clean rebuild
