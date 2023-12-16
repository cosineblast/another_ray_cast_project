#include "map.h"

#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TEXTURE_A_PATH "./assets/beep.png"

#define TEXTURE_B_PATH "./assets/boop.png"

#define TEXTURE_COUNT 2

static SDL_Texture *load_texture(const char *path, SDL_Renderer *renderer) {
    SDL_Surface *surface = IMG_Load(path);

    if (surface == NULL) {
        fprintf(stderr, "failed to load surface %s: %s\n", path,
                SDL_GetError());
        abort();
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture == NULL) {
        fprintf(stderr, "failed to load texture: %s\n", SDL_GetError());
        abort();
    }

    return texture;
}

void initialize_textures(Map *map, SDL_Renderer *renderer) {
    map->textures = malloc(TEXTURE_COUNT * sizeof(SDL_Texture *));

    char *paths[] = {TEXTURE_A_PATH, TEXTURE_B_PATH};

    for (int i = 0; i < TEXTURE_COUNT; i++) {
        map->textures[i] = load_texture(paths[i], renderer);
    }
}

bool map_point_has_wall(struct Map *map, SDL_FPoint point) {
    return map_find_intersecting_wall(map, point) > 0;
}

int8_t map_find_intersecting_wall(struct Map *map, SDL_FPoint point) {
    ssize_t row = (ssize_t)point.y / TILE_SIZE;
    ssize_t col = (ssize_t)point.x / TILE_SIZE;

    if (row >= 0 && col >= 0 && (size_t)row < map->rows &&
        (size_t)col < map->cols) {
        return map->tiles[row * map->cols + col];
    } else {
        return -1;
    }
}

bool inside_map(struct Map *map, SDL_FPoint point) {
    (void)map;
    (void)point;
    abort();
}

bool point_is_walkable(struct Map *map, SDL_FPoint point) {
    return inside_map(map, point) && !map_point_has_wall(map, point);
}

SDL_Texture *map_texture_from_tile_value(Map *map, int8_t tile_value) {
    assert(tile_value <= 2);
    assert(tile_value >= -1);

    if (tile_value <= 0) {
        return NULL;
    }

    size_t texture_index = tile_value - 1;

    assert(map->textures[texture_index] != NULL);

    return map->textures[texture_index];

    return NULL;
}
