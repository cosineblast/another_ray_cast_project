
#include <SDL2/SDL.h>
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


  Map *map = new_sample_map();

  Player player;
  init_sample_player(&player);

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window =
      SDL_CreateWindow("hi there", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_HIDDEN);

  SDL_Renderer *renderer =
    SDL_CreateRenderer(window, -1,
                       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  SDL_ShowWindow(window);


  SDL_Event event;

  int running = 1;

  while (running) {

    while (SDL_PollEvent(&event)) {

      if (event.type == SDL_QUIT) {
        running = 0;
      }

    }

    update_time();

    move_player(renderer, &player);

    SDL_SetRenderDrawColor(renderer, 0x0d, 0x11, 0x17, 0xff);

    SDL_RenderClear(renderer);

    render_player(renderer, &player, map);

    SDL_RenderPresent(renderer);
  }

  free_map(map);

  SDL_DestroyRenderer(renderer);

  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}
