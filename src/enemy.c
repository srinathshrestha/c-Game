/*
 * enemy.c - enemy car spawning and movement
 *
 * theres a fixed pool of MAX_ENEMIES slots.
 * when we need a new enemy we grab an inactive slot, set it up
 * at a random lane at the top, and let it scroll down.
 * when it goes off the bottom we just mark it inactive again.
 * no dynamic memory at all which is nice
 */

#include "enemy.h"
#include "texture.h"

static const char *ENEMY_PATHS[ENEMY_VARIANTS] = {
    SPRITE_ENEMY_1,
    SPRITE_ENEMY_2,
    SPRITE_ENEMY_3
};

void enemy_init(EnemySystem *es, SDL_Renderer *renderer)
{
    for (int i = 0; i < ENEMY_VARIANTS; i++) {
        es->textures[i] = texture_load(renderer, ENEMY_PATHS[i]);
        es->tex_w[i] = CAR_DRAW_W;
        es->tex_h[i] = CAR_DRAW_H;
    }
    enemy_reset(es);
}

void enemy_reset(EnemySystem *es)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
        es->pool[i].active = false;

    es->spawn_timer = 0;
    es->spawn_interval = 60;  // ~1 per second at 60fps
}

// find a free slot, drop a car at the top in a random lane
static void spawn_enemy(EnemySystem *es)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (es->pool[i].active) continue;

        Enemy *e = &es->pool[i];
        e->variant = rand() % ENEMY_VARIANTS;
        e->w = es->tex_w[e->variant];
        e->h = es->tex_h[e->variant];

        int lane = rand() % NUM_LANES;
        int lane_center = ROAD_LEFT + lane * LANE_WIDTH + LANE_WIDTH / 2;
        e->x = (float)(lane_center - e->w / 2);
        e->y = (float)(-e->h);   // start just above the screen
        e->active = true;
        return;
    }
    // all slots full? just skip this spawn, nbd
}

void enemy_update(EnemySystem *es, float speed)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!es->pool[i].active) continue;

        es->pool[i].y += speed;

        // fell off the bottom? recycle it
        if (es->pool[i].y > WINDOW_HEIGHT)
            es->pool[i].active = false;
    }

    // spawn timer
    es->spawn_timer--;
    if (es->spawn_timer <= 0) {
        spawn_enemy(es);
        es->spawn_timer = es->spawn_interval;
    }
}

void enemy_draw(SDL_Renderer *renderer, EnemySystem *es)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &es->pool[i];
        if (!e->active) continue;

        SDL_Texture *tex = es->textures[e->variant];
        if (!tex) continue;

        SDL_Rect dest = { (int)e->x, (int)e->y, e->w, e->h };

        // flip em upside down so they look like theyre coming at you
        SDL_RenderCopyEx(renderer, tex, NULL, &dest,
                         0.0, NULL, SDL_FLIP_VERTICAL);
    }
}

SDL_Rect enemy_get_rect(Enemy *e)
{
    SDL_Rect r = { (int)e->x, (int)e->y, e->w, e->h };
    return r;
}

void enemy_cleanup(EnemySystem *es)
{
    for (int i = 0; i < ENEMY_VARIANTS; i++) {
        if (es->textures[i]) {
            SDL_DestroyTexture(es->textures[i]);
            es->textures[i] = NULL;
        }
    }
}
