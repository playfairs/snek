#include "../registry.h"
#include <stdlib.h>

void shrink_apply(GameContext* game) {
    int remove = (rand() % 5) + 2;
    for (int i = 0; i < remove; i++) {
        if (game->snake.length > 1) {
            game->snake.length--;
        }
    }
}

void shrink_update(GameContext* game) {
    (void)game;
}

void shrink_remove(GameContext* game) {
}
