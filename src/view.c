#include "view.h"

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>

#include "cast.h"
#include "map.h"

#define FOV 1.04719

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

static void get_tile_color(int8_t tile, SDL_Color *color);

// the render_texture_raycast_column and render_colored_raycast_column
// are both functions that take a rectangle (and quite few other things)
// and render something on such rectangle on screen based on
// factors such as the hitting angle and hitting tile
//
// we could implement a better system for choosing the rendering function
// but uh this suffices

// todo: make these arguments into a struct

static void render_texture_raycast_column(Map *map, CastResult *result,
                                          SDL_FRect *column_rectangle,
                                          SDL_Renderer *renderer,
                                          float current_angle);

static void render_colored_raycast_column(Map *map, CastResult *result,
                                          SDL_FRect *column_rectangle,
                                          SDL_Renderer *renderer,
                                          float current_angle);

void player_render_view(SDL_Renderer *renderer, Player *player, Map *map) {
    float current_angle = player->angle + FOV / 2;

    float angle_increment = FOV / (float)SCREEN_WIDTH;

    for (int current_column = 1; current_column <= SCREEN_WIDTH;
         current_column++) {
        CastResult result;

        cast_full(map, (SDL_FPoint){player->x, player->y}, current_angle,
                  &result);

        float distance = result.distance * cos(player->angle - current_angle);

        float height = SCREEN_HEIGHT * 50 / distance;

        float middle_y = (float)SCREEN_HEIGHT / 2 - height / 2;

        SDL_FRect rectangle;
        rectangle.x = current_column;
        rectangle.y = middle_y;
        rectangle.w = 1;
        rectangle.h = height;

        render_texture_raycast_column(map, &result, &rectangle, renderer,
                                      current_angle);

        current_angle -= angle_increment;
    }
}

static void render_texture_raycast_column(Map *map, CastResult *result,
                                          SDL_FRect *column_rectangle,
                                          SDL_Renderer *renderer,
                                          float current_angle) {
    SDL_Texture *texture = map_texture_from_tile_value(map, result->tile);

    if (texture == NULL) {
        SDL_Color color;

        get_tile_color(result->tile, &color);

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawRectF(renderer, column_rectangle);
    } else {
        SDL_Rect source;
        source.x = cast_find_texture_line_offset(result, current_angle);
        source.y = 0;
        source.w = 1;
        source.h = TILE_SIZE;

        SDL_RenderCopyF(renderer, texture, &source, column_rectangle);
    }
}

static void render_colored_raycast_column(Map *map, CastResult *result,
                                          SDL_FRect *column_rectangle,
                                          SDL_Renderer *renderer,
                                          float current_angle) {
    SDL_Color rendered_color;

    get_tile_color(result->tile, &rendered_color);

    SDL_SetRenderDrawColor(renderer, rendered_color.r, rendered_color.g,
                           rendered_color.b, rendered_color.a);

    SDL_RenderFillRectF(renderer, column_rectangle);
}

static void get_tile_color(int8_t tile, SDL_Color *color) {
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
