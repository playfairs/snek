#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "powerups.h"
#include "state.h"
#include "constants.h"
#include "game.h"

void speedboost_apply(GameContext* game) {
    game->current_speed += 5;
}

void speedboost_update(GameContext* game) {
    (void)game;
}

void speedboost_remove(GameContext* game) {
    game->current_speed -= 5;
    if (game->current_speed < 5) game->current_speed = 5;
}

void slowmo_apply(GameContext* game) {
    game->current_speed -= 3;
    if (game->current_speed < 5) game->current_speed = 5;
}

void slowmo_update(GameContext* game) {
    (void)game;
}

void slowmo_remove(GameContext* game) {
    game->current_speed += 3;
}

void doublepoints_apply(GameContext* game) {
    (void)game;
}

void doublepoints_update(GameContext* game) {
    (void)game;
}

void doublepoints_remove(GameContext* game) {
    (void)game;
}

void invincible_apply(GameContext* game) {
    (void)game;
}

void invincible_update(GameContext* game) {
    (void)game;
}

void invincible_remove(GameContext* game) {
    (void)game;
}

void pathfind_apply(GameContext* game) {
    (void)game;
}

void pathfind_update(GameContext* game) {
    (void)game;
}

void pathfind_remove(GameContext* game) {
    (void)game;
}

void frenzy_apply(GameContext* game) {
    (void)game;
}

void frenzy_update(GameContext* game) {
    (void)game;
}

void frenzy_remove(GameContext* game) {
    (void)game;
}

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
    if (game->active_powerup != POWERUP_NONE) {
        switch (game->active_powerup) {
            case POWERUP_SPEED_BOOST:
                speedboost_remove(game);
                break;
            case POWERUP_SLOW_MO:
                slowmo_remove(game);
                break;
            case POWERUP_DOUBLE_POINTS:
                doublepoints_remove(game);
                break;
            case POWERUP_INVINCIBLE:
                invincible_remove(game);
                break;
            case POWERUP_PATHFIND:
                pathfind_remove(game);
                break;
            case POWERUP_FRENZY:
                frenzy_remove(game);
                break;
            default:
                break;
        }
    }
    
    game->active_powerup = type;
    game->powerup_end_time = SDL_GetTicks() / 1000.0 + POWERUP_DURATION;
    
    switch (type) {
        case POWERUP_SPEED_BOOST:
            speedboost_apply(game);
            break;
        case POWERUP_SLOW_MO:
            slowmo_apply(game);
            break;
        case POWERUP_DOUBLE_POINTS:
            doublepoints_apply(game);
            break;
        case POWERUP_INVINCIBLE:
            invincible_apply(game);
            break;
        case POWERUP_PATHFIND:
            pathfind_apply(game);
            break;
        case POWERUP_FRENZY:
            frenzy_apply(game);
            break;
        default:
            break;
    }
}

void update_active_powerup(GameContext* game, double current_time) {
    if (game->active_powerup != POWERUP_NONE && current_time > game->powerup_end_time) {
        switch (game->active_powerup) {
            case POWERUP_SPEED_BOOST:
                speedboost_remove(game);
                break;
            case POWERUP_SLOW_MO:
                slowmo_remove(game);
                break;
            case POWERUP_DOUBLE_POINTS:
                doublepoints_remove(game);
                break;
            case POWERUP_INVINCIBLE:
                invincible_remove(game);
                break;
            case POWERUP_PATHFIND:
                pathfind_remove(game);
                break;
            case POWERUP_FRENZY:
                frenzy_remove(game);
                break;
            default:
                break;
        }
        game->active_powerup = POWERUP_NONE;
    }
}
