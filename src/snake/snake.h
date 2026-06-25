#ifndef SNAKE_H
#define SNAKE_H

#include <SDL2/SDL.h>
#include "core/game/state.h"
#include "core/input/input.h"

void init_snake(Snake* snake, SnakeSkin skin, int start_x, int start_y);
void update_snake_direction(Snake* snake, InputAction action);
int check_self_collision(const Snake* snake);
int check_food_collision(const Snake* snake, const Food* food);
int check_powerup_collision(const Snake* snake, const Powerup* powerup);
void update_snake_position(Snake* snake);
void draw_snake(SDL_Renderer* renderer, const Snake* snake, int glow_active, int frozen);

#endif
