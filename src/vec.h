
#pragma once

#include <SDL2/SDL_rect.h>

typedef struct {
    float x;
    float y;
} FVec2;

void point_add(SDL_FPoint *p, const FVec2 *vec);

float vec_dot(const FVec2 *u, const FVec2 *v);

float vec_norm(const FVec2 *vec);

FVec2 point_difference(SDL_FPoint *p, SDL_FPoint *q);
