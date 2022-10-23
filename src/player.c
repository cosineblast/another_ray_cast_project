
#include "player.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <math.h>
#include "map.h"
#include "timing.h"
#include <stdbool.h>
#include <assert.h>
#include "view.h"
#include "vec.h"
#include "preview.h"

void player_init_sample(Player *player) {
  player->x = 150;
  player->y = 250;
  player->angle = M_PI_2;
}

void player_move(SDL_Renderer *renderer, Player *player) {

    const uint8_t *keyboard_state = SDL_GetKeyboardState(NULL);

    float movement_speed = 600 * timing_time_variation;

    float angular_speed = 1 * timing_time_variation;

    player->angle += angular_speed * keyboard_state[SDL_SCANCODE_A];
    player->angle -= angular_speed * keyboard_state[SDL_SCANCODE_D];

    float angle_sine = sinf(player->angle);
    float angle_cosine = cosf(player->angle);

    player->x += angle_cosine * movement_speed * keyboard_state[SDL_SCANCODE_W];
    player->y -= angle_sine * movement_speed * keyboard_state[SDL_SCANCODE_W];

    player->x -= angle_cosine * movement_speed * keyboard_state[SDL_SCANCODE_S];
    player->y += angle_sine * movement_speed * keyboard_state[SDL_SCANCODE_S];

}


void player_render(SDL_Renderer *renderer, Player *player, Map *map) {
  preview_render(renderer, player, map);
}
