
#pragma once

#include <SDL2/SDL_rect.h>

typedef struct {
    float x;
    float y;
} FVec2;

static void point_add(SDL_FPoint *p, const FVec2 *vec) {
    p->x += vec->x;
    p->y += vec->y;
}
