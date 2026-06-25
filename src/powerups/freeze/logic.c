#include "../registry.h"

void freeze_apply(GameContext* game) {
    game->frozen = 1;
}

void freeze_update(GameContext* game) {
    (void)game;
}

void freeze_remove(GameContext* game) {
    game->frozen = 0;
}
