#include "powerup.h"

void frenzy_apply(GameContext* game) {
    game->current_speed = (int)(game->current_speed * 1.5);
}

void frenzy_update(GameContext* game) {
    (void)game;
}

void frenzy_remove(GameContext* game) {
    game->current_speed = (int)(game->current_speed / 1.5);
    if (game->current_speed < 5) game->current_speed = 5;
}
