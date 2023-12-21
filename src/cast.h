
#pragma once

#include "vec.h"
#include "map.h"

typedef struct {
  SDL_FPoint hit_point;
  SDL_FPoint inside_point;
  int8_t tile;
  int is_vertical;
  float distance;
} CastResult;

typedef struct {
  SDL_FPoint result_point;
  SDL_FPoint inside_point;
  int8_t tile;
} SideCastResult;

#define HORIZONTAL 0
#define VERTICAL 1


typedef void BoundaryCallback_fn(SDL_FPoint *point, void *data);

typedef struct {
  BoundaryCallback_fn *fn;
  void *data;
} BoundaryCallback;

