#include "powerup.h"

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
