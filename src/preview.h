#ifndef PREVIEW_H_
#define PREVIEW_H_

#include <SDL2/SDL.h>
#include "player.h"
#include "map.h"

void preview_render(SDL_Renderer *renderer, Player *player, Map *map);


#endif // PREVIEW_H_
