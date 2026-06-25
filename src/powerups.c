#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "powerups/registry.h"
#include "core/game/state.h"
#include "core/game/constants.h"
#include "core/game/game.h"

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

static void recalculate_speed(GameContext* game) {
    double speed = game->base_speed;
    for (int i = 0; i < game->powerup_count; i++) {
        PowerupType type = game->active_powerups[i].type;
        if (type == POWERUP_SPEED_BOOST) {
            speed *= 1.3;
        } else if (type == POWERUP_FRENZY) {
            speed *= 1.5;
        } else if (type == POWERUP_SLOW_MO) {
            speed -= 3.0;
        }
    }
    if (speed < 5.0) speed = 5.0;
    game->current_speed = (int)speed;
}

void apply_powerup(GameContext* game, PowerupType type) {
    double current_time = SDL_GetTicks() / 1000.0;

    for (int i = 0; i < game->powerup_count; i++) {
        if (game->active_powerups[i].type == type) {
            game->active_powerups[i].end_time = current_time + POWERUP_DURATION;
            return;
        }
    }

    if (game->powerup_count < MAX_POWERUPS) {
        game->active_powerups[game->powerup_count].type = type;
        game->active_powerups[game->powerup_count].end_time = current_time + POWERUP_DURATION;
        game->powerup_count++;
    }
    apply_powerup_effect(game, type);
    recalculate_speed(game);
}

void update_active_powerup(GameContext* game, double current_time) {
    int write_index = 0;
    for (int i = 0; i < game->powerup_count; i++) {
        if (current_time > game->active_powerups[i].end_time) {
            remove_powerup_effect(game, game->active_powerups[i].type);
        } else {
            game->active_powerups[write_index++] = game->active_powerups[i];
        }
    }
    game->powerup_count = write_index;
    recalculate_speed(game);
}
