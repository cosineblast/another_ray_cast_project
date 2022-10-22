#include "map.h"

#include <SDL2/SDL_rect.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct Map *new_sample_map() {
    struct Map *map = malloc(sizeof(*map));

    map->rows = 10;
    map->cols = 10;

    map->tiles = calloc(map->rows * map->cols, sizeof(map->tiles));

    uint8_t sample_map []  = {
        2, 1, 2, 1, 2, 1, 2, 1, 2, 1, //
        1, 0, 0, 0, 0, 0, 0, 0, 0, 2, //
        2, 0, 0, 0, 1, 0, 0, 0, 0, 1, //
        1, 0, 0, 0, 2, 0, 0, 0, 0, 2, //
        2, 0, 0, 0, 1, 0, 0, 0, 0, 1, //
        1, 0, 0, 0, 0, 0, 0, 0, 0, 2, //
        2, 0, 0, 0, 0, 0, 0, 0, 0, 1, //
        1, 0, 0, 0, 0, 0, 0, 0, 0, 2, //
        2, 0, 0, 0, 0, 0, 0, 0, 0, 1, //
        1, 2, 1, 2, 1, 2, 1, 2, 1, 2, //
    };

    memcpy(map->tiles, sample_map, map->rows * map->cols);

    return map;
}

bool point_has_wall(struct Map *map, SDL_FPoint point) {
    return find_intersecting_wall(map, point) > 0;
}

int8_t find_intersecting_wall(struct Map *map, SDL_FPoint point) {
    ssize_t row = (ssize_t) point.y / TILE_SIZE;
    ssize_t col = (ssize_t) point.x / TILE_SIZE;

    if (row >= 0 && col >= 0 &&
        (size_t) row < map->rows && (size_t) col < map->cols
        ) {
        return map->tiles[row * map->cols + col];
    }
    else {
        return -1;
    }
}

bool inside_map(struct Map *map, SDL_FPoint point) {

    (void) map;
    (void) point;
    abort();
}

bool point_is_walkable(struct Map *map, SDL_FPoint point) {
    return inside_map(map, point) && !point_has_wall(map, point);
}


void free_map(struct Map* map) {
    free(map->tiles);
    free(map);
}
