#include "../registry.h"
#include <stdlib.h>

void grow_apply(GameContext* game) {
    int add = (rand() % 5) + 2;
    for (int i = 0; i < add; i++) {
        if (game->snake.length < MAX_SNAKE_LENGTH) {
            game->snake.segments[game->snake.length] = game->snake.segments[game->snake.length - 1];
            game->snake.length++;
        }
    }
}

void grow_update(GameContext* game) {
    (void)game;
}

void grow_remove(GameContext* game) {
}
