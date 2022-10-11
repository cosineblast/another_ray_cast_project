
#pragma once

#include <stdint.h>
#define TILE_SIZE 128

#include <SDL2/SDL.h>
#include <stdbool.h>

struct Map {
    size_t rows;
    size_t cols;

    uint8_t *tiles;
};

typedef struct Map Map;

struct Map *new_sample_map();

bool point_has_wall(struct Map *map, SDL_FPoint point);

int8_t find_intersecting_wall(struct Map *map, SDL_FPoint point);

bool inside_map(struct Map *map, SDL_FPoint point);

bool point_is_walkable(struct Map *map, SDL_FPoint point);

void free_map(struct Map* map);
