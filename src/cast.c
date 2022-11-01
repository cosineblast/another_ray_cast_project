#include "cast.h"
#include "vec.h"
#include "map.h"
#include <SDL2/SDL_rect.h>
#include <stdlib.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480


static void find_vertical_boundary(
  SDL_FPoint point,
  float angle,
  SDL_FPoint *result,
  FVec2 * boundary_distance) {

  float tangent = tanf(angle);
  float cosine = cosf(angle);

  boundary_distance->x = copysignf(TILE_SIZE, cosine);
  boundary_distance->y = boundary_distance->x * tangent;

  float width = (float) ((int)(point.x) % 64);

  float x = point.x - width;

  float top = width * tangent;

  float y = point.y + top;

  if (cosine > 0) {
    y = y - boundary_distance->y;
    x = x + boundary_distance->x;
  }

  result->x = x;
  result->y = y;

  boundary_distance->y *= -1;
}

static void find_horizontal_displacement(float angle, FVec2 *displacement) {
  displacement->x = 0;
  displacement->y = -copysignf(TILE_SIZE/2.0, sinf(angle));
}

static void find_vertical_displacement(float angle, FVec2 *displacement) {
  displacement->x = copysignf(TILE_SIZE/2.0, cosf(angle));
  displacement->y = 0;
}


void find_horizontal_boundary(
  SDL_FPoint point,
  float angle,
  SDL_FPoint *result,
  FVec2 * boundary_distance) {


  float cotangent = 1 / (tanf(angle));
  float sine = sinf(angle);

  boundary_distance->y = copysignf(TILE_SIZE, sine);
  boundary_distance->x = boundary_distance->y * cotangent;

  float height = (float) ((int)(point.y) % 64);

  float y = point.y - height;

  float base = height * cotangent;

  float x = point.x + base;

  if (sine < 0) {
    y = y - boundary_distance->y;
    x = x + boundary_distance->x;
  }

  result->x = x;
  result->y = y;

  boundary_distance->y *= -1;
}

static void full_side_cast(Map *map,
                      const SDL_FPoint start_point,
                      const FVec2 advancement,
                      const FVec2 lookup_displacement,
                      SideCastResult *result,
                      BoundaryCallback *callback
                      ) {

  SDL_FPoint current_point = start_point;

  SDL_FPoint inside_block;

  for (;;) {

    inside_block = current_point;

    point_add(&inside_block, lookup_displacement);

    int8_t tile = map_find_intersecting_wall(map, inside_block);

    if (tile != 0) {
      result->tile = tile;
      break;
    }

    if (current_point.x < 0 || current_point.x > SCREEN_WIDTH ||
         current_point.y < 0 || current_point.y > SCREEN_HEIGHT) {
      result->tile = -1;
      break;
    }

    if (callback) {
        callback->fn(&current_point, callback->data);
    }

    point_add(&current_point, advancement);
  }

  result->inside_point = inside_block;
  result->result_point = current_point;
}

void cast_side(Map *map, SDL_FPoint source_point, int is_vertical, float angle,
               SideCastResult *result,
               BoundaryCallback *callback) {

  SDL_FPoint start_point;

  FVec2 advancement;

  FVec2 displacement;

  if (is_vertical) {
    find_vertical_boundary(source_point, angle, &start_point, &advancement);

    find_vertical_displacement(angle, &displacement);
  } else {
    find_horizontal_boundary(source_point, angle, &start_point, &advancement);

    find_horizontal_displacement(angle, &displacement);
  }

  full_side_cast(map, start_point, advancement, displacement, result,
                 callback);
}


static float point_distance(SDL_FPoint first, SDL_FPoint second) {
  return vec_norm(point_difference(first, second));
}

void cast_result_from_sides(SideCastResult results[2], SDL_FPoint source_point,
                             CastResult *output) {

  float distances[2];

  for (size_t i = 0; i < 2; i++) {
    distances[i] = point_distance(source_point, results[i].result_point);
  }

  size_t shortest_index =
      distances[HORIZONTAL] < distances[VERTICAL] ? HORIZONTAL : VERTICAL;

  output->distance = distances[shortest_index];
  output->hit_point = results[shortest_index].result_point;
  output->inside_point = results[shortest_index].inside_point;
  output->is_vertical = shortest_index == VERTICAL;
  output->tile = results[shortest_index].tile;
}

void cast_full(Map *map, SDL_FPoint source_point, float angle,
               CastResult *output) {

  SideCastResult side_cast_results[2];

  cast_side(map, source_point, HORIZONTAL, angle,
            &side_cast_results[HORIZONTAL], NULL);

  cast_side(map, source_point, VERTICAL, angle, &side_cast_results[VERTICAL],
            NULL);


  cast_result_from_sides(side_cast_results, source_point, output);
}

float cast_find_texture_line_offset(CastResult *result, float cast_angle) {

    float sine = sinf(cast_angle);
    float cosine = cosf(cast_angle);

    if (result->is_vertical) {
        if (cosine < 0) {
          return TILE_SIZE - (int) result->hit_point.y % TILE_SIZE;
        }
        else {
          return (int) result->hit_point.y % TILE_SIZE;
        }
    }
    else {
        if (sine < 0) {
          return TILE_SIZE - (int) result->hit_point.x % TILE_SIZE;
        }
        else {
          return (int) result->hit_point.x % TILE_SIZE;
        }
    }
}
