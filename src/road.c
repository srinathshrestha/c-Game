#include "road.h"

#define COLOR_GRASS_R  58
#define COLOR_GRASS_G  135
#define COLOR_GRASS_B  52
#define COLOR_GRASS_A  255

#define COLOR_ROAD_R   80
#define COLOR_ROAD_G   80
#define COLOR_ROAD_B   80
#define COLOR_ROAD_A   255

#define COLOR_LANE_R   255
#define COLOR_LANE_G   255
#define COLOR_LANE_B   255
#define COLOR_LANE_A   200

void road_init(Road *road)
{
    road->line_offset = 0.0f;
}

void road_update(Road *road, float speed)
{
    road->line_offset += speed;
    float cycle = LANE_LINE_H + LANE_LINE_GAP;
    if (road->line_offset >= cycle)
        road->line_offset -= cycle;
}

// just grass + road + dashes for now
void road_draw(SDL_Renderer *renderer, Road *road)
{
    SDL_SetRenderDrawColor(renderer, COLOR_GRASS_R, COLOR_GRASS_G, COLOR_GRASS_B, COLOR_GRASS_A);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, COLOR_ROAD_R, COLOR_ROAD_G, COLOR_ROAD_B, COLOR_ROAD_A);
    SDL_Rect r = { ROAD_LEFT, 0, ROAD_WIDTH, WINDOW_HEIGHT };
    SDL_RenderFillRect(renderer, &r);

    SDL_SetRenderDrawColor(renderer, COLOR_LANE_R, COLOR_LANE_G, COLOR_LANE_B, COLOR_LANE_A);
    float cycle = LANE_LINE_H + LANE_LINE_GAP;
    for (int lane = 1; lane < NUM_LANES; lane++) {
        int x = ROAD_LEFT + (lane * LANE_WIDTH) - (LANE_LINE_W / 2);
        for (float y = -cycle + road->line_offset; y < WINDOW_HEIGHT; y += cycle) {
            SDL_Rect dash = { x, (int)y, LANE_LINE_W, LANE_LINE_H };
            SDL_RenderFillRect(renderer, &dash);
        }
    }
}
