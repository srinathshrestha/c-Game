/*
 * road.h - the scrolling road background
 * everything is drawn with rectangles, no sprites needed
 */

#ifndef ROAD_H
#define ROAD_H

#include "defs.h"

typedef struct {
    float line_offset;  // how far the dashed lines have scrolled
} Road;

void road_init(Road *road);
void road_update(Road *road, float speed);
void road_draw(SDL_Renderer *renderer, Road *road);

#endif
