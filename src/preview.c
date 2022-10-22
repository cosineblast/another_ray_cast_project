#include "preview.h"

#include <SDL2/SDL.h>
#include "vec.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define HORIZONTAL 0
#define VERTICAL 1

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


static void render_map(SDL_Renderer *renderer, Map *map);
static void render_grid(SDL_Renderer *renderer);
static void render_player2d(SDL_Renderer *renderer, Player *player);
static void render_player_arrow(SDL_Renderer *renderer, Player *player);
static void render_dot(SDL_Renderer *renderer, SDL_FPoint f);
static float point_distance(SDL_FPoint first, SDL_FPoint second);

static void render_boundaries(SDL_Renderer *renderer,
                                  Map *map, Player *player,
                                  int is_vertical,
                       SideCastResult *result);


void render_player_preview(SDL_Renderer *renderer, Player *player, Map *map) {




  render_map(renderer, map);

  render_grid(renderer);
  render_player2d(renderer, player);

  SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);

  SideCastResult horizontal_result,
    vertical_result;

  render_boundaries(renderer, map, player, HORIZONTAL, &horizontal_result);

  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x0, 0xff);

  render_dot(renderer, horizontal_result.result_point);

  SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);

  render_boundaries(renderer, map, player, VERTICAL, &vertical_result);

  SDL_SetRenderDrawColor(renderer, 0x0, 0xff, 0xff, 0xff);

  render_dot(renderer, vertical_result.result_point);

}

static void render_map(SDL_Renderer *renderer, Map *map) {

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

static void render_grid(SDL_Renderer *renderer) {

  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0xff);

  for (int y = 0; y < SCREEN_HEIGHT; y += TILE_SIZE) {

    for (int x = 0; x < SCREEN_WIDTH; x += TILE_SIZE) {

      SDL_RenderDrawLine(renderer, x, y, x + TILE_SIZE, y);
      SDL_RenderDrawLine(renderer, x, y, x , y + TILE_SIZE);
    }

  }

}



static void find_cast_result(SideCastResult results[2], SDL_FPoint start_point,
                             CastResult *output) {

  float distances[2];

  for (size_t i = 0; i < 2; i++) {
    distances[i] = point_distance(start_point, results[i].result_point);
  }


  size_t shortest_index =
    distances[HORIZONTAL] < distances[VERTICAL] ? HORIZONTAL : VERTICAL;

  output->distance = distances[shortest_index];
  output->hit_point = results[shortest_index].result_point;
  output->inside_point = results[shortest_index].inside_point;
  output->is_vertical = shortest_index == VERTICAL;
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

  boundary_distance->y *= -1;
}

void cast_boundaries(SDL_Renderer *renderer,
                      Map *map,
                      const SDL_FPoint *start_point,
                      const FVec2 *advancement,
                      const FVec2 *lookup_displacement,
                      SideCastResult *result
                      ) {

  SDL_FPoint current_point = *start_point;

  SDL_FPoint inside_block;

  for (;;) {

    inside_block = current_point;

    point_add(&inside_block, lookup_displacement);

    int8_t tile = find_intersecting_wall(map, inside_block);

    if (tile != 0) {
      result->tile = tile;
      break;
    }

    if (current_point.x < 0 || current_point.x > SCREEN_WIDTH ||
         current_point.y < 0 || current_point.y > SCREEN_HEIGHT) {
      result->tile = -1;
      break;
    }

    render_dot(renderer, current_point);

    point_add(&current_point, advancement);
  }

  result->inside_point = inside_block;
  result->result_point = current_point;
}

void find_horizontal_displacement(float angle, FVec2 *displacement) {
  displacement->x = 0;
  displacement->y = -copysignf(TILE_SIZE/2.0, sinf(angle));
}

void find_vertical_displacement(float angle, FVec2 *displacement) {
  displacement->x = copysignf(TILE_SIZE/2.0, cosf(angle));
  displacement->y = 0;
}



static float point_distance(SDL_FPoint first, SDL_FPoint second) {

  FVec2 vector = point_difference(&first, &second);

  return vec_norm(&vector);

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


static void render_dot(SDL_Renderer *renderer, SDL_FPoint f) {
    SDL_FRect rect;
    rect.w = 5;
    rect.h = 5;
    rect.x = f.x - rect.w / 2;
    rect.y = f.y - rect.h / 2;

    SDL_RenderFillRectF(renderer, &rect);
}


static void render_player2d(SDL_Renderer *renderer, Player *player) {

  render_player_arrow(renderer, player);

  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0xff);

  SDL_FRect rectangle = {player->x, player->y, 10, 10};

  rectangle.x -= rectangle.w / 2;
  rectangle.y -= rectangle.h / 2;

  SDL_RenderFillRectF(renderer, &rectangle);
}

static void render_boundaries(SDL_Renderer *renderer,
                                  Map *map, Player *player,
                                  int is_vertical,
                       SideCastResult *result) {

  SDL_FPoint start_point;

  FVec2 advancement;

  FVec2 displacement;

  if (is_vertical) {
    find_vertical_boundary((SDL_FPoint){player->x, player->y}, player->angle,
                            &start_point, &advancement);

    find_vertical_displacement(player->angle, &displacement);
  }
  else {
    find_horizontal_boundary((SDL_FPoint){player->x, player->y}, player->angle,
                            &start_point, &advancement);

    find_horizontal_displacement(player->angle, &displacement);
  }

  cast_boundaries(
    renderer,
    map,
    &start_point,
    &advancement,
    &displacement,
    result
  );
}
