#include "preview.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <math.h>

#include "cast.h"
#include "map.h"
#include "vec.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

static void render_map(SDL_Renderer *renderer, Map *map);
static void render_grid(SDL_Renderer *renderer);
static void render_player2d(SDL_Renderer *renderer, Player *player);
static void render_player_arrow(SDL_Renderer *renderer, Player *player);
static void render_dot(SDL_Renderer *renderer, SDL_FPoint f);

static void render_boundaries(SDL_Renderer *renderer, Map *map, Player *player,
                              int is_vertical, SideCastResult *result);

static void utilize_side_casts(SDL_Renderer *renderer, Map *map, Player *player,
                               SideCastResult results[2]);

void preview_render(SDL_Renderer *renderer, Player *player, Map *map) {
    render_map(renderer, map);

    render_grid(renderer);

    render_player2d(renderer, player);

    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);

    SideCastResult results[2], *horizontal_result = &results[HORIZONTAL],
                               *vertical_result = &results[VERTICAL];

    render_boundaries(renderer, map, player, HORIZONTAL, horizontal_result);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x0, 0xff);

    render_dot(renderer, horizontal_result->result_point);

    SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);

    render_boundaries(renderer, map, player, VERTICAL, vertical_result);

    SDL_SetRenderDrawColor(renderer, 0x0, 0xff, 0xff, 0xff);

    render_dot(renderer, vertical_result->result_point);

    utilize_side_casts(renderer, map, player, results);
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
            SDL_RenderDrawLine(renderer, x, y, x, y + TILE_SIZE);
        }
    }
}

static void render_player2d(SDL_Renderer *renderer, Player *player) {
    render_player_arrow(renderer, player);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0xff);

    SDL_FRect rectangle = {player->x, player->y, 10, 10};

    rectangle.x -= rectangle.w / 2;
    rectangle.y -= rectangle.h / 2;

    SDL_RenderFillRectF(renderer, &rectangle);
}

static void render_player_arrow(SDL_Renderer *renderer, Player *player) {
    float arrow_size = 50;

    float sine = sinf(player->angle);
    float cosine = cosf(player->angle);

    float arrow_head_x = player->x + arrow_size * cosine;
    float arrow_head_y = player->y - arrow_size * sine;

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
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

static void on_boundary_callback(SDL_FPoint *boundary_point, void *data) {
    SDL_Renderer *renderer = data;

    render_dot(renderer, *boundary_point);
}

static void render_boundaries(SDL_Renderer *renderer, Map *map, Player *player,
                              int is_vertical, SideCastResult *result) {
    BoundaryCallback callback = {on_boundary_callback, renderer};

    cast_side(map, (SDL_FPoint){player->x, player->y}, is_vertical,
              player->angle, result, &callback);
}

static void draw_result_line(SDL_Renderer *renderer, Player *player,
                             CastResult *cast_result) {
    SDL_Color colors[2] = {{0xff, 0x88, 0xff, 0xff}, {0xff, 0xff, 0x88, 0xff}

    };

    SDL_Color line_color = colors[cast_result->is_vertical];

    SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b,
                           line_color.a);

    SDL_RenderDrawLineF(renderer, player->x, player->y,
                        cast_result->hit_point.x, cast_result->hit_point.y);
}

static void render_tile_texture(SDL_Renderer *renderer, SDL_Texture *texture) {
    SDL_Rect source;
    source.h = TILE_SIZE;
    source.w = TILE_SIZE;
    source.x = 0;
    source.y = 0;

    SDL_FRect destination;
    destination.h = TILE_SIZE;
    destination.w = TILE_SIZE;
    destination.x = 0;
    destination.y = 0;

    SDL_RenderCopyF(renderer, texture, &source, &destination);
}

static void render_tile_texture_line(SDL_Renderer *renderer,
                                     CastResult *cast_result, Player *player) {
    float offset = cast_find_texture_line_offset(cast_result, player->angle);

    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);

    SDL_RenderDrawLineF(renderer, offset, 0, offset, TILE_SIZE);
}

static void utilize_side_casts(SDL_Renderer *renderer, Map *map, Player *player,
                               SideCastResult side_results[2]) {
    CastResult cast_result;

    cast_result_from_sides(side_results, (SDL_FPoint){player->x, player->y},
                           &cast_result);

    draw_result_line(renderer, player, &cast_result);

    SDL_Texture *texture = map_texture_from_tile_value(map, cast_result.tile);

    if (texture != NULL) {
        render_tile_texture(renderer, texture);

        render_tile_texture_line(renderer, &cast_result, player);
    }
}
