// player car - just arrow keys for now
#include "player.h"
#include "texture.h"

void player_init(Player *player, SDL_Renderer *renderer)
{
    player->texture = texture_load(renderer, SPRITE_PLAYER);
    // use fixed draw size instead of actual sprite dimensions
    player->w = CAR_DRAW_W;
    player->h = CAR_DRAW_H;
    }
    player_reset(player);
}

void player_reset(Player *player)
{
    player->x = (float)(ROAD_LEFT + ROAD_WIDTH / 2 - player->w / 2);
    player->y = (float)PLAYER_START_Y;
}

void player_handle_input(Player *player, const Uint8 *keys)
{
    if (keys[SDL_SCANCODE_LEFT])
        player->x -= PLAYER_SPEED;
    if (keys[SDL_SCANCODE_RIGHT])
        player->x += PLAYER_SPEED;

    if (player->x < ROAD_LEFT)
        player->x = ROAD_LEFT;
    if (player->x + player->w > ROAD_RIGHT)
        player->x = ROAD_RIGHT - player->w;
}

void player_draw(SDL_Renderer *renderer, Player *player)
{
    if (!player->texture) return;
    SDL_Rect dest = { (int)player->x, (int)player->y, player->w, player->h };
    SDL_RenderCopy(renderer, player->texture, NULL, &dest);
}

void player_cleanup(Player *player)
{
    // use fixed draw size instead of actual sprite dimensions
    player->w = CAR_DRAW_W;
    player->h = CAR_DRAW_H;
        SDL_DestroyTexture(player->texture);
        player->texture = NULL;
    }
}
