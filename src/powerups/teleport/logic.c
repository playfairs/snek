#include "../registry.h"
#include <stdlib.h>

static int position_equal(const Position* a, const Position* b) {
    return a->x == b->x && a->y == b->y;
}

static int is_inside_game(int x, int y) {
    return x >= 0 && x < DIS_WIDTH && y >= GAME_AREA_TOP && y < GAME_AREA_BOTTOM;
}

static int position_conflicts(GameContext* game, const Position* pos) {
    if (game->food.active && position_equal(pos, &game->food.pos)) {
        return 1;
    }
    if (game->secondary_food.active && position_equal(pos, &game->secondary_food.pos)) {
        return 1;
    }
    if (game->powerup.active && position_equal(pos, &game->powerup.pos)) {
        return 1;
    }
    return 0;
}

void teleport_apply(GameContext* game) {
    int max_x = DIS_WIDTH / SNAKE_BLOCK;
    int max_y = GAME_AREA_HEIGHT / SNAKE_BLOCK;
    Position directions[4] = {
        {SNAKE_BLOCK, 0},
        {-SNAKE_BLOCK, 0},
        {0, SNAKE_BLOCK},
        {0, -SNAKE_BLOCK}
    };

    for (int attempts = 0; attempts < 100; attempts++) {
        int rx = rand() % max_x;
        int ry = rand() % max_y;
        Position head = {rx * SNAKE_BLOCK, GAME_AREA_TOP + ry * SNAKE_BLOCK};

        if (position_conflicts(game, &head)) {
            continue;
        }

        if (game->snake.length == 1) {
            game->snake.segments[0] = head;
            return;
        }

        for (int di = 0; di < 4; di++) {
            Position dir = directions[di];
            int ok = 1;
            for (int i = 0; i < game->snake.length; i++) {
                Position candidate = {
                    head.x - (game->snake.length - 1 - i) * dir.x,
                    head.y - (game->snake.length - 1 - i) * dir.y
                };
                if (!is_inside_game(candidate.x, candidate.y) || position_conflicts(game, &candidate)) {
                    ok = 0;
                    break;
                }
                for (int j = 0; j < game->snake.length; j++) {
                    if (i != j && position_equal(&candidate, &game->snake.segments[j])) {
                        ok = 0;
                        break;
                    }
                }
                if (!ok) break;
            }
            if (!ok) {
                continue;
            }

            for (int i = 0; i < game->snake.length; i++) {
                game->snake.segments[i] = (Position){
                    head.x - (game->snake.length - 1 - i) * dir.x,
                    head.y - (game->snake.length - 1 - i) * dir.y
                };
            }
            return;
        }
    }
}

void teleport_update(GameContext* game) {
    (void)game;
}

void teleport_remove(GameContext* game) {
    (void)game;
}
