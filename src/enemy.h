/*
 * enemy.h - enemy cars that scroll toward you
 *
 * uses a fixed-size pool so theres no malloc/free to worry about.
 * inactive slots get reused when new enemies spawn
 */

#ifndef ENEMY_H
#define ENEMY_H

#include "defs.h"

typedef struct {
    float x, y;
    int w, h;
    int variant;     // which sprite (0..2)
    bool active;
} Enemy;

typedef struct {
    Enemy pool[MAX_ENEMIES];
    SDL_Texture *textures[ENEMY_VARIANTS];
    int tex_w[ENEMY_VARIANTS];   // not really needed since we use CAR_DRAW_* but
    int tex_h[ENEMY_VARIANTS];   // keeping it in case we want per-variant sizes later
    int spawn_timer;
    int spawn_interval;          // frames between spawns, gets shorter over time
} EnemySystem;

void     enemy_init(EnemySystem *es, SDL_Renderer *renderer);
void     enemy_reset(EnemySystem *es);
void     enemy_update(EnemySystem *es, float speed);
void     enemy_draw(SDL_Renderer *renderer, EnemySystem *es);
SDL_Rect enemy_get_rect(Enemy *e);
void     enemy_cleanup(EnemySystem *es);

#endif
