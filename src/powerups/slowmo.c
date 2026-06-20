#include "powerup.h"

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
