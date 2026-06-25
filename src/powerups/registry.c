#include "powerups/registry.h"
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

void speedboost_apply(GameContext* game);
void speedboost_update(GameContext* game);
void speedboost_remove(GameContext* game);
void slowmo_apply(GameContext* game);
void slowmo_update(GameContext* game);
void slowmo_remove(GameContext* game);
void doublepoints_apply(GameContext* game);
void doublepoints_update(GameContext* game);
void doublepoints_remove(GameContext* game);
void invincible_apply(GameContext* game);
void invincible_update(GameContext* game);
void invincible_remove(GameContext* game);
void frenzy_apply(GameContext* game);
void frenzy_update(GameContext* game);
void frenzy_remove(GameContext* game);
void pathfind_apply(GameContext* game);
void pathfind_update(GameContext* game);
void pathfind_remove(GameContext* game);

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

PowerupInfo get_powerup_info(PowerupType type) {
    switch (type) {
        case POWERUP_SPEED_BOOST:
            return (PowerupInfo){POWERUP_SPEED_BOOST, "SPEED BOOST", LIGHT_BLUE_R, LIGHT_BLUE_G, LIGHT_BLUE_B, POWERUP_DURATION};
        case POWERUP_SLOW_MO:
            return (PowerupInfo){POWERUP_SLOW_MO, "SLOW MOTION", GREEN_R, GREEN_G, GREEN_B, POWERUP_DURATION};
        case POWERUP_DOUBLE_POINTS:
            return (PowerupInfo){POWERUP_DOUBLE_POINTS, "2X POINTS", ACCENT_R, ACCENT_G, ACCENT_B, POWERUP_DURATION};
        case POWERUP_INVINCIBLE:
            return (PowerupInfo){POWERUP_INVINCIBLE, "INVINCIBLE", LIGHT_RED_R, LIGHT_RED_G, LIGHT_RED_B, POWERUP_DURATION};
        case POWERUP_PATHFIND:
            return (PowerupInfo){POWERUP_PATHFIND, "PATHFIND", 100, 255, 150, POWERUP_DURATION};
        case POWERUP_FRENZY:
            return (PowerupInfo){POWERUP_FRENZY, "FRENZY", 255, 150, 50, POWERUP_DURATION};
        default:
            return (PowerupInfo){POWERUP_NONE, "NONE", WHITE_R, WHITE_G, WHITE_B, 0};
    }
}

void apply_powerup_effect(GameContext* game, PowerupType type) {
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
        case POWERUP_FRENZY:
            frenzy_apply(game);
            break;
        case POWERUP_PATHFIND:
            pathfind_apply(game);
            break;
        default:
            break;
    }
}

void update_powerup_effect(GameContext* game, PowerupType type) {
    switch (type) {
        case POWERUP_SPEED_BOOST:
            speedboost_update(game);
            break;
        case POWERUP_SLOW_MO:
            slowmo_update(game);
            break;
        case POWERUP_DOUBLE_POINTS:
            doublepoints_update(game);
            break;
        case POWERUP_INVINCIBLE:
            invincible_update(game);
            break;
        case POWERUP_FRENZY:
            frenzy_update(game);
            break;
        case POWERUP_PATHFIND:
            pathfind_update(game);
            break;
        default:
            break;
    }
}

void remove_powerup_effect(GameContext* game, PowerupType type) {
    switch (type) {
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
        case POWERUP_FRENZY:
            frenzy_remove(game);
            break;
        case POWERUP_PATHFIND:
            pathfind_remove(game);
            break;
        default:
            break;
    }
}
