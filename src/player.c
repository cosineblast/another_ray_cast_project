
#include "player.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <math.h>
#include "map.h"
#include "timing.h"
#include <stdbool.h>
#include <assert.h>

typedef struct {
  float distance;
  int8_t tile_value;
  SDL_FPoint touch_point;
}RayCastResult ;

void get_tile_color(int8_t tile, SDL_Color *color);


void cast_ray(float initial_x, float initial_y, float angle, Map *map,
              RayCastResult *result);

void init_sample_player(Player *player) {
  player->x = 150;
  player->y = 250;
  player->angle = M_PI_2;
}

static void render_player_arrow(SDL_Renderer *renderer, Player *player) {


  float arrow_size = 50;

  float sine = sinf(player->angle);
  float cosine = cosf(player->angle);

  float arrow_head_x = player->x + arrow_size * cosine;
  float arrow_head_y = player->y - arrow_size * sine;

  SDL_SetRenderDrawColor(renderer,0x00, 0x00, 0xff, 0xff);
  SDL_RenderDrawLine(renderer, player->x, player->y, arrow_head_x,
arrow_head_y);


}

void render_player2d(SDL_Renderer *renderer, Player *player) {

  render_player_arrow(renderer, player);

  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0xff);

  SDL_FRect rectangle = {player->x, player->y, 10, 10};

  rectangle.x -= rectangle.w / 2;
  rectangle.y -= rectangle.h / 2;

  SDL_RenderFillRectF(renderer, &rectangle);
}

void move_player(SDL_Renderer *renderer, Player *player) {

    const uint8_t *keyboard_state = SDL_GetKeyboardState(NULL);

    float movement_speed = 600 * time_variation;

    float angular_speed = 4 * time_variation;

    player->angle += angular_speed * keyboard_state[SDL_SCANCODE_A];
    player->angle -= angular_speed * keyboard_state[SDL_SCANCODE_D];

    float angle_sine = sinf(player->angle);
    float angle_cosine = cosf(player->angle);

    player->x += angle_cosine * movement_speed * keyboard_state[SDL_SCANCODE_W];
    player->y -= angle_sine * movement_speed * keyboard_state[SDL_SCANCODE_W];

    player->x -= angle_cosine * movement_speed * keyboard_state[SDL_SCANCODE_S];
    player->y += angle_sine * movement_speed * keyboard_state[SDL_SCANCODE_S];

}

#define FOV 1.04719

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

static void render_player_view(SDL_Renderer *renderer, Player *player, Map *map) {

  float current_angle = player->angle + FOV / 2;

  float angle_increment = FOV / (float)SCREEN_WIDTH;

  for (int current_column = 1; current_column <= SCREEN_WIDTH;
       current_column++) {

    RayCastResult result;

    cast_ray(player->x, player->y, current_angle, map, &result);

    float distance = result.distance * cos(player->angle - current_angle);

    float height = SCREEN_HEIGHT * 50 / distance;

    float mid_y = (float)SCREEN_HEIGHT / 2 - height / 2;

    SDL_FRect rect;
    rect.x = current_column;
    rect.y = mid_y;
    rect.w = 1;
    rect.h = height;

    SDL_Color color;

    get_tile_color(result.tile_value, &color);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRectF(renderer, &rect);



    current_angle -= angle_increment;
  }
}

void get_tile_color(int8_t tile, SDL_Color *color) {

  assert(tile != 0);
  assert(tile >= -1);
  assert(tile <= 2);

  if (tile == -1) {
    *color = (SDL_Color){0xff, 0xff, 0xff, 0xff};
  }
  else {
    static const SDL_Color colors[] = {
      [1] = {0x28, 0x2c, 0x34, 0xff},
      [2] = {0xf7, 0x81, 0xA9, 0xff}
    };

    *color = colors[tile];
  }
}


static bool point_in_bounds(float x, float y) {
    return x >= 0 && x <= SCREEN_WIDTH &&
        y >= 0 && y <= SCREEN_HEIGHT;
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

void render_player(SDL_Renderer *renderer, Player *player, Map *map) {
    render_player_view(renderer, player, map);
}
