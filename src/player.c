
#include "player.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include "map.h"
#include "timing.h"
#include <stdbool.h>
#include <assert.h>
#include "view.h"
#include "vec.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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

void render_grid(SDL_Renderer *renderer) {

  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0xff);

  for (int y = 0; y < SCREEN_HEIGHT; y += TILE_SIZE) {

    for (int x = 0; x < SCREEN_WIDTH; x += TILE_SIZE) {

      SDL_RenderDrawLine(renderer, x, y, x + TILE_SIZE, y);
      SDL_RenderDrawLine(renderer, x, y, x , y + TILE_SIZE);
    }

  }

}

void render_dot(SDL_Renderer *renderer, SDL_FPoint f) {
    SDL_FRect rect;
    rect.w = 5;
    rect.h = 5;
    rect.x = f.x - rect.w / 2;
    rect.y = f.y - rect.h / 2;

    SDL_RenderFillRectF(renderer, &rect);
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

}

void find_vertical_boundary(
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
}



void render_horizontal_boundaries(SDL_Renderer *renderer, Player *player) {

  SDL_FPoint current_point;

  FVec2 boundary_distance;

  find_horizontal_boundary(
    (SDL_FPoint){player->x, player->y},
    player->angle, &current_point, &boundary_distance);

  for (;;) {

    if (current_point.x < 0 || current_point.x > SCREEN_WIDTH ||
         current_point.y < 0 || current_point.y > SCREEN_HEIGHT) {
      break;
    }

    render_dot(renderer, current_point);

    current_point.y -= boundary_distance.y;
    current_point.x += boundary_distance.x;
  }

}

void render_vertical_boundaries(SDL_Renderer *renderer, Player *player) {

  SDL_FPoint current_point;

  FVec2 boundary_distance;

  find_vertical_boundary(
    (SDL_FPoint){player->x, player->y},
    player->angle, &current_point, &boundary_distance);

  for (;;) {

    if (current_point.x < 0 || current_point.x > SCREEN_WIDTH ||
         current_point.y < 0 || current_point.y > SCREEN_HEIGHT) {
      break;
    }

    render_dot(renderer, current_point);

    current_point.y -= boundary_distance.y;
    current_point.x += boundary_distance.x;
  }

}

void render_map(SDL_Renderer *renderer, Map *map) {

  int y = 0;
  int x = 0;

  SDL_FRect rect;
  rect.w = TILE_SIZE;
  rect.h = TILE_SIZE;

  for (int row = 0; row < map->rows && y < SCREEN_HEIGHT; row++) {
    for (int col = 0; col < map->cols && x < SCREEN_HEIGHT; col++) {

      int8_t tile = map->tiles[row * map->cols + col];

      if (tile > 0) {
        rect.x = x;
        rect.y = y;
        SDL_SetRenderDrawColor(renderer, 0x28, 0x2C, 0x34, 0xff);
        SDL_RenderFillRectF(renderer, &rect);
      }

      x += TILE_SIZE;
    }
    x = 0;
    y += TILE_SIZE;
  }

}

void render_player(SDL_Renderer *renderer, Player *player, Map *map) {

  render_map(renderer, map);

  render_grid(renderer);
  render_player2d(renderer, player);

  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);

  render_horizontal_boundaries(renderer, player);

  SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);

  render_vertical_boundaries(renderer, player);
}
