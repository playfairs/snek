#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "../include/powerups.h"
#include "../include/state.h"
#include "../include/constants.h"

void spawn_powerup(Powerup* powerup) {
    int max_x = (DIS_WIDTH - SNAKE_BLOCK) / SNAKE_BLOCK;
    int max_y = (DIS_HEIGHT - GAME_AREA_TOP - SNAKE_BLOCK) / SNAKE_BLOCK;
    
    powerup->pos.x = (rand() % max_x) * SNAKE_BLOCK;
    powerup->pos.y = GAME_AREA_TOP + (rand() % max_y) * SNAKE_BLOCK;
    powerup->type = rand() % MAX_POWERUPS;
    powerup->spawn_time = SDL_GetTicks() / 1000.0;
    powerup->active = 1;
}

void update_powerup(Powerup* powerup, double current_time) {
    if (powerup->active && current_time - powerup->spawn_time > 10) {
        powerup->active = 0;
    }
}

int check_powerup_collision(Snake* snake, Powerup* powerup) {
    if (!powerup->active) {
        return 0;
    }
    
    Position head = snake->segments[snake->length - 1];
    
    if (abs(head.x - powerup->pos.x) < SNAKE_BLOCK && 
        abs(head.y - powerup->pos.y) < SNAKE_BLOCK) {
        return 1;
    }
    return 0;
}

void apply_powerup(GameContext* game, PowerupType type) {
    game->active_powerup = type;
    game->powerup_end_time = SDL_GetTicks() / 1000.0 + POWERUP_DURATION;
}

void update_active_powerup(GameContext* game, double current_time) {
    if (game->active_powerup != POWERUP_NONE && current_time > game->powerup_end_time) {
        game->active_powerup = POWERUP_NONE;
    }
}
