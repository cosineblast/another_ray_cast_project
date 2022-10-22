
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "map.h"
#include "player.h"
#include "timing.h"



int main() {


  SDL_Init(SDL_INIT_VIDEO);

  IMG_Init(IMG_INIT_PNG);


  SDL_Window *window =
      SDL_CreateWindow("hi there", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_HIDDEN);

  SDL_Renderer *renderer =
    SDL_CreateRenderer(window, -1,
                       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  SDL_ShowWindow(window);

  Map *map = map_new_sample(renderer);

  Player player;
  player_init_sample(&player);


  SDL_Event event;

  int running = 1;

  while (running) {

    while (SDL_PollEvent(&event)) {

      if (event.type == SDL_QUIT) {
        running = 0;
      }

    }

    timing_update_time();

    player_move(renderer, &player);

    SDL_SetRenderDrawColor(renderer, 0x0d, 0x11, 0x17, 0xff);

    SDL_RenderClear(renderer);

    player_render(renderer, &player, map);

    SDL_RenderPresent(renderer);
  }

  map_free(map);

  SDL_DestroyRenderer(renderer);

  SDL_DestroyWindow(window);

  IMG_Quit();

  SDL_Quit();

  return 0;
}
