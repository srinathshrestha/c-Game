/*
 * texture.c - handles loading images and drawing text
 *
 * every module that needs to draw a sprite or some text
 * calls into these functions. keeps the SDL boilerplate
 * in one place so we dont have to think about it elsewhere
 */

#include "texture.h"

// load a png (or jpg/bmp) and give back a gpu texture
// returns NULL if it cant find the file or something goes wrong
SDL_Texture *texture_load(SDL_Renderer *renderer, const char *filepath)
{
    SDL_Surface *surface = IMG_Load(filepath);
    if (!surface) {
        printf("ERROR: cant load image '%s': %s\n", filepath, IMG_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
        printf("ERROR: texture creation failed for '%s': %s\n", filepath, SDL_GetError());

    SDL_FreeSurface(surface);  // texture has the pixels now, surface is useless
    return texture;
}

// open a .ttf font at the given pt size
TTF_Font *texture_load_font(const char *filepath, int size)
{
    TTF_Font *font = TTF_OpenFont(filepath, size);
    if (!font)
        printf("ERROR: cant load font '%s': %s\n", filepath, TTF_GetError());
    return font;
}

/*
 * draw text at (x,y) - this creates a texture every single call
 * which is kinda wasteful but for a small HUD its totally fine.
 * if we ever had like 50 text elements we'd want to cache these
 */
void texture_render_text(SDL_Renderer *renderer, TTF_Font *font,
                         const char *text, int x, int y, SDL_Color color)
{
    if (!font || !text) return;

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) {
        printf("ERROR: text render failed: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    // cleanup - yeah we alloc and free every frame, sue me
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// grab the pixel dimensions of a texture
void texture_get_size(SDL_Texture *texture, int *w, int *h)
{
    if (!texture) {
        if (w) *w = 0;
        if (h) *h = 0;
        return;
    }
    SDL_QueryTexture(texture, NULL, NULL, w, h);
}

// draw a texture at whatever size you want (ignores original dimensions)
void texture_draw_scaled(SDL_Renderer *renderer, SDL_Texture *texture,
                         int x, int y, int w, int h)
{
    if (!texture) return;
    SDL_Rect dest = { x, y, w, h };
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}
