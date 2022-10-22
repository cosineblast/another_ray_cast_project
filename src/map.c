#include "map.h"

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TEXTURE_A_PATH "./assets/beep.png"

#define TEXTURE_B_PATH "./assets/boop.png"

#define TEXTURE_COUNT 2


static void initialize_textures(Map *map, SDL_Renderer *renderer);


struct Map *new_sample_map(SDL_Renderer *renderer) {
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

    initialize_textures(map, renderer);

    return map;
}

static SDL_Texture *load_texture(const char *path, SDL_Renderer *renderer) {

    SDL_Surface *surface = IMG_Load(path);

    if (surface == NULL) {
        fprintf(stderr, "failed to load surface %s: %s\n", path, SDL_GetError());
        abort();
    }

    SDL_Texture *texture
        = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture == NULL) {
        fprintf(stderr, "failed to load texture: %s\n", SDL_GetError());
        abort();
    }

    return texture;
}

static void initialize_textures(Map *map, SDL_Renderer *renderer) {

    map->textures = malloc(TEXTURE_COUNT * sizeof(SDL_Texture*));

    char *paths[] = {TEXTURE_A_PATH, TEXTURE_B_PATH};

    for (int i = 0; i < TEXTURE_COUNT; i++) {
        map->textures[i] = load_texture(paths[i], renderer);
    }
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

SDL_Texture *map_texture_from_tile_value(Map *map, int8_t tile_value) {

    assert(tile_value <= 2);
    assert(tile_value >= -1);

    if (tile_value <= 0) {
        return NULL;
    }


    assert(map->textures[tile_value] != NULL);

    return map->textures[tile_value];

    return NULL;

}
