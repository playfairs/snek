#include "powerups/registry.h"
#include <stdio.h>
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
void magnet_apply(GameContext* game);
void magnet_update(GameContext* game);
void magnet_remove(GameContext* game);
void freeze_apply(GameContext* game);
void freeze_update(GameContext* game);
void freeze_remove(GameContext* game);
void shrink_apply(GameContext* game);
void shrink_update(GameContext* game);
void shrink_remove(GameContext* game);
void grow_apply(GameContext* game);
void grow_update(GameContext* game);
void grow_remove(GameContext* game);
void teleport_apply(GameContext* game);
void teleport_update(GameContext* game);
void teleport_remove(GameContext* game);
void split_apply(GameContext* game);
void split_update(GameContext* game);
void split_remove(GameContext* game);

void spawn_powerup(Powerup* powerup) {
    int max_x = DIS_WIDTH / SNAKE_BLOCK;
    int max_y = GAME_AREA_HEIGHT / SNAKE_BLOCK;

    powerup->pos.x = (rand() % max_x) * SNAKE_BLOCK;
    powerup->pos.y = GAME_AREA_TOP + (rand() % max_y) * SNAKE_BLOCK;

    PowerupType allowed[] = {
        POWERUP_SPEED_BOOST,
        POWERUP_SLOW_MO,
        POWERUP_DOUBLE_POINTS,
        POWERUP_INVINCIBLE,
        POWERUP_PATHFIND,
        POWERUP_FRENZY,
        POWERUP_MAGNET,
        POWERUP_FREEZE,
        POWERUP_SHRINK,
        POWERUP_GROW,
        POWERUP_TELEPORT,
        POWERUP_SPLIT
    };
    int choice = rand() % (sizeof(allowed) / sizeof(allowed[0]));
    powerup->type = allowed[choice];
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

    double duration = POWERUP_DURATION;
    switch (type) {
        case POWERUP_SPEED_BOOST: duration = POWERUP_DURATION_SPEED_BOOST; break;
        case POWERUP_SLOW_MO: duration = POWERUP_DURATION_SLOW_MO; break;
        case POWERUP_DOUBLE_POINTS: duration = POWERUP_DURATION_DOUBLE_POINTS; break;
        case POWERUP_INVINCIBLE: duration = POWERUP_DURATION_INVINCIBLE; break;
        case POWERUP_PATHFIND: duration = POWERUP_DURATION_PATHFIND; break;
        case POWERUP_FRENZY: duration = POWERUP_DURATION_FRENZY; break;
        case POWERUP_MAGNET: duration = POWERUP_DURATION_MAGNET; break;
        case POWERUP_FREEZE: duration = POWERUP_DURATION_FREEZE; break;
        case POWERUP_SHRINK: duration = POWERUP_DURATION_SHRINK; break;
        case POWERUP_GROW: duration = POWERUP_DURATION_GROW; break;
        case POWERUP_TELEPORT: duration = POWERUP_DURATION_TELEPORT; break;
        case POWERUP_SPLIT: duration = POWERUP_DURATION_SPLIT; break;
        default: duration = POWERUP_DURATION; break;
    }

    if (duration <= 0.0) {
        apply_powerup_effect(game, type);
        PowerupInfo info = get_powerup_info(type);
        if (info.name) {
            snprintf(game->last_powerup_text, sizeof(game->last_powerup_text), "%s PICKED UP", info.name);
            game->last_powerup_display_time = current_time;
        }
        recalculate_speed(game);
        return;
    }

    for (int i = 0; i < game->powerup_count; i++) {
        if (game->active_powerups[i].type == type) {
            game->active_powerups[i].end_time = current_time + duration;
            return;
        }
    }

    if (game->powerup_count < MAX_POWERUPS) {
        game->active_powerups[game->powerup_count].type = type;
        game->active_powerups[game->powerup_count].end_time = current_time + duration;
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
            return (PowerupInfo){POWERUP_SPEED_BOOST, "SPEED BOOST", LIGHT_BLUE_R, LIGHT_BLUE_G, LIGHT_BLUE_B, POWERUP_DURATION_SPEED_BOOST};
        case POWERUP_SLOW_MO:
            return (PowerupInfo){POWERUP_SLOW_MO, "SLOW MOTION", 145, 80, 255, POWERUP_DURATION_SLOW_MO};
        case POWERUP_DOUBLE_POINTS:
            return (PowerupInfo){POWERUP_DOUBLE_POINTS, "2X POINTS", 255, 215, 70, POWERUP_DURATION_DOUBLE_POINTS};
        case POWERUP_INVINCIBLE:
            return (PowerupInfo){POWERUP_INVINCIBLE, "INVINCIBLE", 255, 80, 180, POWERUP_DURATION_INVINCIBLE};
        case POWERUP_PATHFIND:
            return (PowerupInfo){POWERUP_PATHFIND, "PATHFIND", 130, 255, 120, POWERUP_DURATION_PATHFIND};
        case POWERUP_FRENZY:
            return (PowerupInfo){POWERUP_FRENZY, "FRENZY", 255, 140, 35, POWERUP_DURATION_FRENZY};
        case POWERUP_MAGNET:
            return (PowerupInfo){POWERUP_MAGNET, "MAGNET", 220, 45, 45, POWERUP_DURATION_MAGNET};
        case POWERUP_FREEZE:
            return (PowerupInfo){POWERUP_FREEZE, "FREEZE", 155, 220, 255, POWERUP_DURATION_FREEZE};
        case POWERUP_SHRINK:
            return (PowerupInfo){POWERUP_SHRINK, "SHRINK", 128, 0, 32, POWERUP_DURATION_SHRINK};
        case POWERUP_GROW:
            return (PowerupInfo){POWERUP_GROW, "GROW", 240, 230, 130, POWERUP_DURATION_GROW};
        case POWERUP_TELEPORT:
            return (PowerupInfo){POWERUP_TELEPORT, "TELEPORT", 170, 220, 255, POWERUP_DURATION_TELEPORT};
        case POWERUP_SPLIT:
            return (PowerupInfo){POWERUP_SPLIT, "SPLIT", 160, 0, 35, POWERUP_DURATION_SPLIT};
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
        case POWERUP_MAGNET:
            magnet_apply(game);
            break;
        case POWERUP_FREEZE:
            freeze_apply(game);
            break;
        case POWERUP_SHRINK:
            shrink_apply(game);
            break;
        case POWERUP_GROW:
            grow_apply(game);
            break;
        case POWERUP_TELEPORT:
            teleport_apply(game);
            break;
        case POWERUP_SPLIT:
            split_apply(game);
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
        case POWERUP_MAGNET:
            magnet_update(game);
            break;
        case POWERUP_FREEZE:
            freeze_update(game);
            break;
        case POWERUP_SHRINK:
            shrink_update(game);
            break;
        case POWERUP_GROW:
            grow_update(game);
            break;
        case POWERUP_TELEPORT:
            teleport_update(game);
            break;
        case POWERUP_SPLIT:
            split_update(game);
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
        case POWERUP_MAGNET:
            magnet_remove(game);
            break;
        case POWERUP_FREEZE:
            freeze_remove(game);
            break;
        case POWERUP_SHRINK:
            shrink_remove(game);
            break;
        case POWERUP_GROW:
            grow_remove(game);
            break;
        case POWERUP_TELEPORT:
            teleport_remove(game);
            break;
        case POWERUP_SPLIT:
            split_remove(game);
            break;
        default:
            break;
    }
}
