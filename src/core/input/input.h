#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include "core/game/game.h"
#include "core/game/state.h"

typedef enum {
    INPUT_NONE,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_QUIT,
    INPUT_PAUSE,
    INPUT_ENTER,
    INPUT_ESCAPE
} InputAction;

InputAction handle_input_snake(SDL_Event* event, int current_dx, int current_dy);
InputAction handle_input_menu(SDL_Event* event);
InputAction handle_input_settings(SDL_Event* event, int* current_option, int max_options);

#endif
