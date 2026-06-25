#include "../registry.h"

void speedboost_apply(GameContext* game) {
    game->current_speed = (int)(game->current_speed * 1.3);
}

void speedboost_update(GameContext* game) {
    (void)game;
}

void speedboost_remove(GameContext* game) {
    game->current_speed = (int)(game->current_speed / 1.3);
    if (game->current_speed < 5) game->current_speed = 5;
}
