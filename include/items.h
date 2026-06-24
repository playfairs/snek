#ifndef ITEMS_H
#define ITEMS_H

#include "state.h"
#include "constants.h"
#include <SDL2/SDL.h>

ItemType random_item_type(void);
void draw_apple(SDL_Renderer* renderer, Food* food);
void draw_banana(SDL_Renderer* renderer, Food* food);
void draw_bomb(SDL_Renderer* renderer, Food* food);

#endif
