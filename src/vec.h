
#pragma once

#include <SDL2/SDL_rect.h>

typedef struct {
    float x;
    float y;
} FVec2;

static SDL_FPoint point_plus(SDL_FPoint p, FVec2 vec) {
    return (SDL_FPoint){p.x + vec.x, p.y + vec.y};
}
