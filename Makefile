CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
SRC = $(wildcard src/*.c)
TARGET = car_game

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -o $(TARGET) $(SRC) $(SDL_LIBS)

clean:
	rm -f $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
