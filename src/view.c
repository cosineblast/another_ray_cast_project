#include "view.h"

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>

#include "cast.h"
#include "map.h"

typedef struct {
    float distance;
    int8_t tile_value;
    SDL_FPoint touch_point;
} RayCastResult;

void get_tile_color(int8_t tile, SDL_Color *color);

#define FOV 1.04719

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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

        SDL_Texture *texture = map_texture_from_tile_value(map, result.tile);

        if (texture == NULL) {
            SDL_Color color;

            get_tile_color(result.tile, &color);

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                   color.a);
            SDL_RenderDrawRectF(renderer, &rectangle);
        } else {
            SDL_Rect source;
            source.x = cast_find_texture_line_offset(&result, current_angle);
            source.y = 0;
            source.w = 1;
            source.h = TILE_SIZE;

            SDL_RenderCopyF(renderer, texture, &source, &rectangle);
        }

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
