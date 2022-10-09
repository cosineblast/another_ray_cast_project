
#include "player.h"
#include <SDL2/SDL_rect.h>
#include <math.h>
#include "timing.h"
#include <stdbool.h>

float distance_to_wall(float x, float y, float angle);

void init_sample_player(Player *player) {
  player->x = 100;
  player->y = 200;
  player->angle = M_PI_2;
}

static void render_player_arrow(SDL_Renderer *renderer, Player *player) {


  float arrow_size = 50;

  float sine = sinf(player->angle);
  float cosine = cosf(player->angle);

  float arrow_head_x = player->x + arrow_size * cosine;
  float arrow_head_y = player->y - arrow_size * sine;

  SDL_SetRenderDrawColor(renderer,0x00, 0x00, 0xff, 0xff);
  SDL_RenderDrawLine(renderer, player->x, player->y, arrow_head_x,
arrow_head_y);


}

void render_player2d(SDL_Renderer *renderer, Player *player) {

  render_player_arrow(renderer, player);

  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0xff);

  SDL_FRect rectangle = {player->x, player->y, 10, 10};

  rectangle.x -= rectangle.w / 2;
  rectangle.y -= rectangle.h / 2;

  SDL_RenderFillRectF(renderer, &rectangle);
}

void move_player(SDL_Renderer *renderer, Player *player) {

    const uint8_t *keyboard_state = SDL_GetKeyboardState(NULL);

    float movement_speed = 600 * time_variation;

    float angular_speed = 4 * time_variation;

    player->angle += angular_speed * keyboard_state[SDL_SCANCODE_A];
    player->angle -= angular_speed * keyboard_state[SDL_SCANCODE_D];

    float angle_sine = sinf(player->angle);
    float angle_cosine = cosf(player->angle);

    player->x += angle_cosine * movement_speed * keyboard_state[SDL_SCANCODE_W];
    player->y -= angle_sine * movement_speed * keyboard_state[SDL_SCANCODE_W];

    player->x -= angle_cosine * movement_speed * keyboard_state[SDL_SCANCODE_S];
    player->y += angle_sine * movement_speed * keyboard_state[SDL_SCANCODE_S];

}

#define FOV 1.04719

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

void render_player_view(SDL_Renderer *renderer, Player *player) {


    float current_angle = player->angle - FOV / 2;

    float angle_increment = FOV / (float) SCREEN_WIDTH;

    for (int current_column = 1; current_column <= SCREEN_WIDTH;
         current_column++) {

        float distance = distance_to_wall(player->x, player->y, current_angle);

        float height = SCREEN_HEIGHT * 50 / distance;

        float mid_y = (float) SCREEN_HEIGHT / 2 - height / 2;

        SDL_FRect rect;
        rect.x = current_column;
        rect.y = mid_y;
        rect.w = 1;
        rect.h = height;

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
        SDL_RenderDrawRectF(renderer, &rect);

        rect.y = rect.y + height;
        rect.h = SCREEN_HEIGHT - rect.y;
        SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
        SDL_RenderDrawRectF(renderer, &rect);

        rect.y = 0;
        rect.h = mid_y;
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderDrawRectF(renderer, &rect);


        current_angle += angle_increment;
    }

}

static bool point_in_bounds(float x, float y) {
    return x >= 0 && x <= SCREEN_WIDTH &&
        y >= 0 && y <= SCREEN_HEIGHT;
}

float distance_to_wall(float initial_x, float initial_y, float angle) {

    float const increment_unit = 1;

    float const increment_x = increment_unit * cosf(angle);
    float const increment_y = increment_unit * sinf(angle);

    float total_distance = 0;

    float x = initial_x, y = initial_y;

    while (point_in_bounds(x, y)) {
        total_distance += increment_unit;
        x += increment_x;
        y -= increment_y;
    }

    return total_distance;
}


void render_player(SDL_Renderer *renderer, Player *player) {
    render_player_view(renderer, player);
}
