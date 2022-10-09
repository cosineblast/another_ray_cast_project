#include "time.h"

#include <SDL2/SDL.h>

uint32_t last_time_measure;
uint32_t debug_last_second_measure;
float time_variation;


void update_time() {

  uint32_t last = last_time_measure;
  uint32_t current = SDL_GetTicks();

  time_variation = (current - last) / 1000.0;

  last_time_measure = current;

  if (current > debug_last_second_measure + 1000) {
    printf("FPS: %f\n", 1.0/time_variation);
    debug_last_second_measure = current;
  }
}
