/*
 * player.c - player car movement and rendering
 *
 * two control schemes:
 *   arrows     - the normal way
 *   J/L + W/S  - for people who prefer that i guess
 *
 * car is clamped to road horizontally and screen vertically
 * so you cant just drive off into the grass lol
 */

#include "player.h"
#include "texture.h"

// vertical bounds - dont let the player go off screen
#define PLAYER_TOP_LIMIT    60
#define PLAYER_BOTTOM_LIMIT (WINDOW_HEIGHT - 20)

void player_init(Player *player, SDL_Renderer *renderer)
{
    player->texture = texture_load(renderer, SPRITE_PLAYER);
    player->w = CAR_DRAW_W;
    player->h = CAR_DRAW_H;
    player_reset(player);
}

// plop the car back in the middle of the road
void player_reset(Player *player)
{
    player->x = (float)(ROAD_LEFT + ROAD_WIDTH / 2 - player->w / 2);
    player->y = (float)PLAYER_START_Y;
}

void player_handle_input(Player *player, const Uint8 *keys)
{
    // horizontal: left/right arrows or J/L
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_J])
        player->x -= PLAYER_SPEED;
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_L])
        player->x += PLAYER_SPEED;

    // vertical: up/down arrows or W/S
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
        player->y -= PLAYER_SPEED;
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
        player->y += PLAYER_SPEED;

    // keep car on the road
    if (player->x < ROAD_LEFT)
        player->x = ROAD_LEFT;
    if (player->x + player->w > ROAD_RIGHT)
        player->x = ROAD_RIGHT - player->w;

    // keep car on screen vertically
    if (player->y < PLAYER_TOP_LIMIT)
        player->y = PLAYER_TOP_LIMIT;
    if (player->y + player->h > PLAYER_BOTTOM_LIMIT)
        player->y = PLAYER_BOTTOM_LIMIT - player->h;
}

// bounding box for collision checks (truncates float->int, good enough)
SDL_Rect player_get_rect(Player *player)
{
    SDL_Rect r = { (int)player->x, (int)player->y, player->w, player->h };
    return r;
}

void player_draw(SDL_Renderer *renderer, Player *player)
{
    if (!player->texture) return;
    SDL_Rect dest = { (int)player->x, (int)player->y, player->w, player->h };
    SDL_RenderCopy(renderer, player->texture, NULL, &dest);
}

void player_cleanup(Player *player)
{
    if (player->texture) {
        SDL_DestroyTexture(player->texture);
        player->texture = NULL;
    }
}
