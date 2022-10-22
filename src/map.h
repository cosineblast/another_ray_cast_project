
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

struct Map *new_sample_map(SDL_Renderer *renderer);

bool point_has_wall(struct Map *map, SDL_FPoint point);

int8_t find_intersecting_wall(struct Map *map, SDL_FPoint point);

bool inside_map(struct Map *map, SDL_FPoint point);

bool point_is_walkable(struct Map *map, SDL_FPoint point);

void free_map(struct Map* map);

SDL_Texture *map_texture_from_tile_value(Map *map, int8_t tile_value);
