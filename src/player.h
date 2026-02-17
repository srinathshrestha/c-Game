/*
 * player.h - the player's car
 * controls: arrow keys or J/L for left/right, W/S for up/down
 */
#ifndef PLAYER_H
#define PLAYER_H
#include "defs.h"

typedef struct {
    float x, y;
    int w, h;
    SDL_Texture *texture;
} Player;

void     player_init(Player *player, SDL_Renderer *renderer);
void     player_handle_input(Player *player, const Uint8 *keys);
SDL_Rect player_get_rect(Player *player);
void     player_draw(SDL_Renderer *renderer, Player *player);
void     player_reset(Player *player);
void     player_cleanup(Player *player);

#endif
