
#pragma once

#include <SDL2/SDL.h>

#include "map.h"

typedef struct {
  float x;
  float y;
  float angle;
} Player;


void player_init_sample(Player *player);

void player_render(SDL_Renderer *renderer, Player *player, Map *map);

void player_move(SDL_Renderer *renderer, Player *player);
