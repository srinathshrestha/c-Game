#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define WINDOW_WIDTH   800
#define WINDOW_HEIGHT  600
#define WINDOW_TITLE   "Car Racing"

#define FPS            60
#define FRAME_DELAY    (1000 / FPS)

// road layout
#define ROAD_LEFT      200
#define ROAD_RIGHT     600
#define ROAD_WIDTH     (ROAD_RIGHT - ROAD_LEFT)
#define NUM_LANES      3
#define LANE_WIDTH     (ROAD_WIDTH / NUM_LANES)

#define LANE_LINE_W    4
#define LANE_LINE_H    40
#define LANE_LINE_GAP  30

typedef enum {
    STATE_START,
    STATE_PLAYING,
    STATE_GAMEOVER
} GameState;

#endif
