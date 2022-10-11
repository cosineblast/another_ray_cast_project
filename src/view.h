
#pragma once

#include <SDL2/SDL_render.h>
#include "map.h"
#include "player.h"

void render_player_view(SDL_Renderer *renderer, Player *player, Map *map);
