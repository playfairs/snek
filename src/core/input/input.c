#include <SDL2/SDL.h>
#include "core/input/input.h"

InputAction handle_input_snake(SDL_Event* event, int current_dx, int current_dy) {
    if (event->type == SDL_QUIT) {
        return INPUT_QUIT;
    }

    if (event->type == SDL_KEYDOWN) {
        SDL_Keycode key = event->key.keysym.sym;

        if (key == SDLK_ESCAPE) {
            return INPUT_PAUSE;
        }

        if (key == SDLK_e && (SDL_GetModState() & KMOD_CTRL)) {
            return INPUT_PAUSE;
        }

        if ((key == SDLK_LEFT || key == SDLK_a) && current_dx == 0) {
            return INPUT_LEFT;
        }
        if ((key == SDLK_RIGHT || key == SDLK_d) && current_dx == 0) {
            return INPUT_RIGHT;
        }
        if ((key == SDLK_UP || key == SDLK_w) && current_dy == 0) {
            return INPUT_UP;
        }
        if ((key == SDLK_DOWN || key == SDLK_s) && current_dy == 0) {
            return INPUT_DOWN;
        }
    }

    return INPUT_NONE;
}

InputAction handle_input_menu(SDL_Event* event) {
    if (event->type == SDL_QUIT) {
        return INPUT_QUIT;
    }

    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_ESCAPE) {
            return INPUT_QUIT;
        }
    }

    return INPUT_NONE;
}

InputAction handle_input_settings(SDL_Event* event, int* current_option, int max_options) {
    if (event->type == SDL_QUIT) {
        return INPUT_QUIT;
    }

    if (event->type == SDL_KEYDOWN) {
        SDL_Keycode key = event->key.keysym.sym;

        if (key == SDLK_ESCAPE) {
            return INPUT_ESCAPE;
        }
        if (key == SDLK_UP || key == SDLK_w) {
            *current_option = (*current_option - 1 + max_options) % max_options;
        }
        if (key == SDLK_DOWN || key == SDLK_s) {
            *current_option = (*current_option + 1) % max_options;
        }
        if (key == SDLK_RETURN || key == SDLK_SPACE) {
            return INPUT_ENTER;
        }
    }

    return INPUT_NONE;
}
