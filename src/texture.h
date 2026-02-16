/*
 * texture.h - sprite loading + text drawing helpers
 * basically wraps SDL_image and SDL_ttf so we dont repeat boilerplate
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "defs.h"

SDL_Texture *texture_load(SDL_Renderer *renderer, const char *filepath);
TTF_Font    *texture_load_font(const char *filepath, int size);

void texture_render_text(SDL_Renderer *renderer, TTF_Font *font,
                         const char *text, int x, int y, SDL_Color color);

void texture_get_size(SDL_Texture *texture, int *w, int *h);

void texture_draw_scaled(SDL_Renderer *renderer, SDL_Texture *texture,
                         int x, int y, int w, int h);

#endif
