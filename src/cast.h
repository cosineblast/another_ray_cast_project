
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


void cast_side(Map *map, SDL_FPoint source_point, int is_vertical, float angle,
               SideCastResult *result,
               BoundaryCallback *callback);

void cast_full(Map *map, SDL_FPoint source_point, float angle,
               CastResult *result);

void cast_result_from_sides(SideCastResult results[2], SDL_FPoint source_point,
                             CastResult *output);
