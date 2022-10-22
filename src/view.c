#include "view.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>

typedef struct {
  float distance;
  int8_t tile_value;
  SDL_FPoint touch_point;
} RayCastResult;

void get_tile_color(int8_t tile, SDL_Color *color);

void cast_ray(float initial_x, float initial_y, float angle, Map *map,
              RayCastResult *result);

#define FOV 1.04719

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

void render_player_view(SDL_Renderer *renderer, Player *player, Map *map) {

  float current_angle = player->angle + FOV / 2;

  float angle_increment = FOV / (float)SCREEN_WIDTH;

  for (int current_column = 1; current_column <= SCREEN_WIDTH;
       current_column++) {

    RayCastResult result;

    cast_ray(player->x, player->y, current_angle, map, &result);

    // Fator de correção linear de olho de peixe
    float distance = result.distance * cos(player->angle - current_angle);

    float height = SCREEN_HEIGHT * 50 / distance;

    float middle_y = (float)SCREEN_HEIGHT / 2 - height / 2;

    SDL_FRect rectangle;
    rectangle.x = current_column;
    rectangle.y = middle_y;
    rectangle.w = 1;
    rectangle.h = height;

    SDL_Color color;

    get_tile_color(result.tile_value, &color);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRectF(renderer, &rectangle);

    current_angle -= angle_increment;
  }
}

void get_tile_color(int8_t tile, SDL_Color *color) {

  assert(tile != 0);
  assert(tile >= -1);
  assert(tile <= 2);

  if (tile == -1) {
    *color = (SDL_Color){0xff, 0xff, 0xff, 0xff};
  } else {
    static const SDL_Color colors[] = {
        [1] = {0x28, 0x2c, 0x34, 0xff}, [2] = {0xf7, 0x81, 0xA9, 0xff}};

    *color = colors[tile];
  }
}

static bool point_in_bounds(float x, float y) {
  return x >= 0 && x <= SCREEN_WIDTH && y >= 0 && y <= SCREEN_HEIGHT;
}

void cast_ray(float initial_x, float initial_y, float angle, Map *map,
              RayCastResult *result) {

  float const increment_unit = 1;

  float const increment_x = increment_unit * cosf(angle);
  float const increment_y = increment_unit * sinf(angle);

  float total_distance = 0;

  SDL_FPoint point = {initial_x, initial_y};

  for (;;) {

    if (fabsf(point.x) > 1000 || fabs(point.y) > 1000) {
      result->tile_value = -1;
      break;
    }

    int8_t tile = find_intersecting_wall(map, point);

    if (tile > 0) {
      result->tile_value = tile;
      break;
    }

    point.x += increment_x;
    point.y -= increment_y;
    total_distance += increment_unit;
  }

  result->touch_point = point;
  result->distance = total_distance;
}
