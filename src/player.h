
#pragma once

#include <SDL2/SDL.h>

typedef struct {
  float x;
  float y;
  float angle;
} Player;


void init_sample_player(Player *player);
void render_player(SDL_Renderer *renderer, Player *player);
void move_player(SDL_Renderer *renderer, Player *player);
