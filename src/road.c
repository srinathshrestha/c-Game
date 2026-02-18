/*
 * road.c - draws the whole road scene with SDL rectangles
 *
 * layers from back to front:
 *   grass -> shoulder strips -> asphalt -> edge lines -> lane dashes
 *
 * the dashed lines scroll down to make it look like you're moving.
 * no textures needed which is nice
 */

#include "road.h"

#define SHOULDER_WIDTH  15
#define EDGE_LINE_W     3

// shoulder = that dark strip between grass and road
#define COLOR_SHOULDER_R  50
#define COLOR_SHOULDER_G  50
#define COLOR_SHOULDER_B  50

#define COLOR_EDGE_R  255
#define COLOR_EDGE_G  255
#define COLOR_EDGE_B  255

void road_init(Road *road)
{
    road->line_offset = 0.0f;
}

// scroll the lane dashes, wraps around so it loops cleanly
void road_update(Road *road, float speed)
{
    road->line_offset += speed;

    float cycle = LANE_LINE_H + LANE_LINE_GAP;
    if (road->line_offset >= cycle)
        road->line_offset -= cycle;
}

static void draw_grass(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer,
        COLOR_GRASS_R, COLOR_GRASS_G, COLOR_GRASS_B, COLOR_GRASS_A);
    SDL_RenderClear(renderer);
}

static void draw_road_surface(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer,
        COLOR_ROAD_R, COLOR_ROAD_G, COLOR_ROAD_B, COLOR_ROAD_A);
    SDL_Rect r = { ROAD_LEFT, 0, ROAD_WIDTH, WINDOW_HEIGHT };
    SDL_RenderFillRect(renderer, &r);
}

// dark strips on each side, makes the road edges pop a bit more
static void draw_shoulders(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer,
        COLOR_SHOULDER_R, COLOR_SHOULDER_G, COLOR_SHOULDER_B, 255);

    SDL_Rect left = { ROAD_LEFT - SHOULDER_WIDTH, 0,
                      SHOULDER_WIDTH, WINDOW_HEIGHT };
    SDL_RenderFillRect(renderer, &left);

    SDL_Rect right = { ROAD_RIGHT, 0,
                       SHOULDER_WIDTH, WINDOW_HEIGHT };
    SDL_RenderFillRect(renderer, &right);
}

// solid white lines at the road boundary
static void draw_edge_lines(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, COLOR_EDGE_R, COLOR_EDGE_G, COLOR_EDGE_B, 255);

    SDL_Rect left  = { ROAD_LEFT - EDGE_LINE_W, 0, EDGE_LINE_W, WINDOW_HEIGHT };
    SDL_Rect right = { ROAD_RIGHT, 0, EDGE_LINE_W, WINDOW_HEIGHT };
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);
}

// the scrolling dashed dividers between lanes
static void draw_lane_lines(SDL_Renderer *renderer, float offset)
{
    SDL_SetRenderDrawColor(renderer,
        COLOR_LANE_R, COLOR_LANE_G, COLOR_LANE_B, COLOR_LANE_A);

    float cycle = LANE_LINE_H + LANE_LINE_GAP;

    for (int lane = 1; lane < NUM_LANES; lane++) {
        int x = ROAD_LEFT + (lane * LANE_WIDTH) - (LANE_LINE_W / 2);

        for (float y = -cycle + offset; y < WINDOW_HEIGHT; y += cycle) {
            SDL_Rect dash = { x, (int)y, LANE_LINE_W, LANE_LINE_H };
            SDL_RenderFillRect(renderer, &dash);
        }
    }
}

void road_draw(SDL_Renderer *renderer, Road *road)
{
    draw_grass(renderer);
    draw_shoulders(renderer);
    draw_road_surface(renderer);
    draw_edge_lines(renderer);
    draw_lane_lines(renderer, road->line_offset);
}
