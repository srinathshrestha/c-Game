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
void     player_draw(SDL_Renderer *renderer, Player *player);
void     player_reset(Player *player);
void     player_cleanup(Player *player);

#endif
