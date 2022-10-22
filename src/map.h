
#pragma once

#include <SDL2/SDL_render.h>
#include <stdint.h>
#define TILE_SIZE 64

#include <SDL2/SDL.h>
#include <stdbool.h>

struct Map {
    size_t rows;
    size_t cols;

    uint8_t *tiles;

    SDL_Texture **textures;
};


typedef struct Map Map;

struct Map *map_new_sample(SDL_Renderer *renderer);

bool map_point_has_wall(struct Map *map, SDL_FPoint point);

int8_t map_find_intersecting_wall(struct Map *map, SDL_FPoint point);

bool map_point_is_inside_map(struct Map *map, SDL_FPoint point);

bool map_point_is_walkable(struct Map *map, SDL_FPoint point);

void map_free(struct Map* map);

SDL_Texture *map_texture_from_tile_value(Map *map, int8_t tile_value);
