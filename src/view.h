
#pragma once

#include <SDL2/SDL_render.h>
#include "map.h"
#include "player.h"

void player_render_view(SDL_Renderer *renderer, Player *player, Map *map);
