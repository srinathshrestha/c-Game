/*
 * hud.c - all the UI drawing (score, lives, title screen, game over)
 *
 * lives are shown as tiny car sprites in the top-right corner.
 * when you get hit one disappears. looks way better than just "Lives: 3"
 */

#include "hud.h"
#include "texture.h"

#define ICON_W    20
#define ICON_H    40
#define ICON_GAP  5

// tiny car icons for each remaining life
static void draw_life_icons(SDL_Renderer *renderer,
                            SDL_Texture *car_icon, int lives)
{
    if (!car_icon) return;

    int total_w = STARTING_LIVES * (ICON_W + ICON_GAP);
    int start_x = WINDOW_WIDTH - 10 - total_w;

    for (int i = 0; i < lives; i++) {
        SDL_Rect icon = {
            start_x + i * (ICON_W + ICON_GAP), 8,
            ICON_W, ICON_H
        };
        SDL_RenderCopy(renderer, car_icon, NULL, &icon);
    }
}

// helper to center text horizontally. saves a lot of math in the callers
static void draw_centered_text(SDL_Renderer *renderer, TTF_Font *font,
                               const char *text, int y, SDL_Color color)
{
    if (!font || !text) return;

    int tw = 0;
    TTF_SizeText(font, text, &tw, NULL);
    texture_render_text(renderer, font, text, (WINDOW_WIDTH - tw) / 2, y, color);
}

// in-game HUD: score on the left, life icons on the right
void hud_draw_playing(SDL_Renderer *renderer, TTF_Font *font,
                      SDL_Texture *car_icon, int score, int lives)
{
    SDL_Color white = { 255, 255, 255, 255 };
    char buf[64];

    snprintf(buf, sizeof(buf), "Score: %d", score);
    texture_render_text(renderer, font, buf, 10, 10, white);

    draw_life_icons(renderer, car_icon, lives);
}

void hud_draw_start(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Color yellow = { 255, 255, 0, 255 };
    SDL_Color white  = { 255, 255, 255, 255 };
    SDL_Color gray   = { 180, 180, 180, 255 };

    int cy = WINDOW_HEIGHT / 2;

    draw_centered_text(renderer, font, "CAR RACING", cy - 80, yellow);
    draw_centered_text(renderer, font, "Press SPACE to Start", cy - 20, white);
    draw_centered_text(renderer, font, "Arrow keys or J/L to steer", cy + 30, gray);
    draw_centered_text(renderer, font, "W/S or Up/Down to move", cy + 60, gray);
    draw_centered_text(renderer, font, "ESC to quit", cy + 100, gray);
}

void hud_draw_paused(SDL_Renderer *renderer, TTF_Font *font)
{
    // dim the screen a bit so the text stands out
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);
    SDL_Rect overlay = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    SDL_RenderFillRect(renderer, &overlay);

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color gray  = { 180, 180, 180, 255 };
    int cy = WINDOW_HEIGHT / 2;

    draw_centered_text(renderer, font, "PAUSED", cy - 30, white);
    draw_centered_text(renderer, font, "Press P to resume", cy + 20, gray);
}

void hud_draw_gameover(SDL_Renderer *renderer, TTF_Font *font,
                       int score, int high_score)
{
    SDL_Color red    = { 255, 50, 50, 255 };
    SDL_Color white  = { 255, 255, 255, 255 };
    SDL_Color yellow = { 255, 255, 0, 255 };
    char buf[64];

    int cy = WINDOW_HEIGHT / 2;

    draw_centered_text(renderer, font, "GAME OVER", cy - 80, red);

    snprintf(buf, sizeof(buf), "Score: %d", score);
    draw_centered_text(renderer, font, buf, cy - 30, white);

    snprintf(buf, sizeof(buf), "High Score: %d", high_score);
    draw_centered_text(renderer, font, buf, cy + 10, yellow);

    draw_centered_text(renderer, font, "Press SPACE to Restart", cy + 60, white);
}
