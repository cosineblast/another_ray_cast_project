#include "vec.h"

#include <SDL2/SDL_rect.h>
#include <math.h>

void point_add(SDL_FPoint *p, const FVec2 vec) {
    p->x += vec.x;
    p->y += vec.y;
}

float vec_dot(const FVec2 u, const FVec2 v) { return u.x * v.x + u.y * v.y; }

float vec_norm(const FVec2 vec) { return sqrtf(vec.x * vec.x + vec.y * vec.y); }

FVec2 point_difference(SDL_FPoint p, SDL_FPoint q) {
    FVec2 result;

    result.x = q.x - p.x;
    result.y = q.y - p.y;

    return result;
}
