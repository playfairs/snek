#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "powerups.h"
#include "state.h"
#include "constants.h"
#include "game.h"

void spawn_powerup(Powerup* powerup) {
    int max_x = DIS_WIDTH / SNAKE_BLOCK;
    int max_y = GAME_AREA_HEIGHT / SNAKE_BLOCK;
    
    powerup->pos.x = (rand() % max_x) * SNAKE_BLOCK;
    powerup->pos.y = GAME_AREA_TOP + (rand() % max_y) * SNAKE_BLOCK;
    powerup->type = rand() % (POWERUP_NONE);
    powerup->spawn_time = SDL_GetTicks() / 1000.0;
    powerup->active = 1;
}

void update_powerup(Powerup* powerup, double current_time) {
    if (powerup->active && current_time - powerup->spawn_time > POWERUP_ITEM_LIFETIME) {
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
    if (game->active_powerup != POWERUP_NONE) {
        remove_powerup_effect(game, game->active_powerup);
    }
    
    game->active_powerup = type;
    game->powerup_end_time = SDL_GetTicks() / 1000.0 + POWERUP_DURATION;
    
    apply_powerup_effect(game, type);
}

void update_active_powerup(GameContext* game, double current_time) {
    if (game->active_powerup != POWERUP_NONE && current_time > game->powerup_end_time) {
        remove_powerup_effect(game, game->active_powerup);
        game->active_powerup = POWERUP_NONE;
    }
}
