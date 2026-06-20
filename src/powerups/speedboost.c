#include "powerup.h"

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
