#include "../registry.h"

void split_apply(GameContext* game) {
    int original_length = game->snake.length;
    if (original_length <= 2) {
        return;
    }

    int cut = original_length / 2;
    int new_length = original_length - cut;
    if (new_length < 1) {
        new_length = 1;
    }

    for (int i = 0; i < new_length; i++) {
        game->snake.segments[i] = game->snake.segments[i + cut];
    }
    game->snake.length = new_length;
}

void split_update(GameContext* game) {
    (void)game;
}

void split_remove(GameContext* game) {
    (void)game;
}
