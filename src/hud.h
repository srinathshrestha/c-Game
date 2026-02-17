/*
 * hud.h - on-screen ui stuff (score, lives, menus)
 * split out from game.c so that file doesnt get huge
 */

#ifndef HUD_H
#define HUD_H

#include "defs.h"

void hud_draw_playing(SDL_Renderer *renderer, TTF_Font *font,
                      SDL_Texture *car_icon, int score, int lives);

void hud_draw_start(SDL_Renderer *renderer, TTF_Font *font);

void hud_draw_gameover(SDL_Renderer *renderer, TTF_Font *font,
                       int score, int high_score);

#endif
