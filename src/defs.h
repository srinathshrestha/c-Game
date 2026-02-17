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

#define ROAD_LEFT      200
#define ROAD_RIGHT     600
#define ROAD_WIDTH     (ROAD_RIGHT - ROAD_LEFT)
#define NUM_LANES      3
#define LANE_WIDTH     (ROAD_WIDTH / NUM_LANES)

#define LANE_LINE_W    4
#define LANE_LINE_H    40
#define LANE_LINE_GAP  30

#define PLAYER_SPEED   6
#define PLAYER_START_Y (WINDOW_HEIGHT - 130)

#define MAX_ENEMIES    6
#define ENEMY_BASE_SPEED  4
#define ENEMY_VARIANTS 3

#define STARTING_LIVES 3
#define SCORE_PER_FRAME    1
#define DIFFICULTY_STEP    500
#define SPEED_INCREMENT    1

#define COLOR_GRASS_R  20
#define COLOR_GRASS_G  20
#define COLOR_GRASS_B  20
#define COLOR_GRASS_A  255

#define COLOR_ROAD_R   30
#define COLOR_ROAD_G   30
#define COLOR_ROAD_B   30
#define COLOR_ROAD_A   255

#define COLOR_LANE_R   255
#define COLOR_LANE_G   255
#define COLOR_LANE_B   255
#define COLOR_LANE_A   200

#define SPRITE_PLAYER      "assets/sprites/player_car.png"
#define SPRITE_ENEMY_1     "assets/sprites/enemy_car_1.png"
#define SPRITE_ENEMY_2     "assets/sprites/enemy_car_2.png"
#define SPRITE_ENEMY_3     "assets/sprites/enemy_car_3.png"
#define FONT_PATH          "assets/font.ttf"
#define FONT_SIZE          24

typedef enum {
    STATE_START,
    STATE_PLAYING,
    STATE_GAMEOVER
} GameState;

#endif
