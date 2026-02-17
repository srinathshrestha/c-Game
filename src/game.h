/*
 * game.h - main game struct and the public api
 * basically everything lives in here. call game_init, game_run, game_cleanup.
 */

#ifndef GAME_H
#define GAME_H

#include "defs.h"
#include "player.h"
#include "enemy.h"
#include "road.h"
#include "audio.h"

typedef struct Game {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    TTF_Font     *font;

    Player      player;
    EnemySystem enemies;
    Road        road;
    Audio       audio;

    GameState state;
    int       score;
    int       lives;
    int       high_score;

    float enemy_speed;
    int   difficulty_level;
    bool  running;
} Game;

bool game_init(Game *game);
void game_run(Game *game);        // blocks until player quits
void game_cleanup(Game *game);

#endif
