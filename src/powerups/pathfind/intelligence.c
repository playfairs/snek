#include "../registry.h"
#include <math.h>
#include <limits.h>

static int has_active_powerup(GameContext* game, PowerupType type) {
    for (int i = 0; i < game->powerup_count; i++) {
        if (game->active_powerups[i].type == type) {
            return 1;
        }
    }
    return 0;
}

static int is_powerup_target_type(PowerupType type) {
    return type == POWERUP_PATHFIND || type == POWERUP_DOUBLE_POINTS || type == POWERUP_FRENZY;
}

static int wrap_value(int value, int limit) {
    if (value < 0) {
        return value + limit;
    }
    if (value >= limit) {
        return value - limit;
    }
    return value;
}

static Position wrap_position(int x, int y) {
    Position pos;
    int grid_width = DIS_WIDTH / SNAKE_BLOCK;
    int grid_height = GAME_AREA_HEIGHT / SNAKE_BLOCK;

    int wrapped_x = wrap_value(x / SNAKE_BLOCK, grid_width);
    int wrapped_y = wrap_value((y - GAME_AREA_TOP) / SNAKE_BLOCK, grid_height);

    pos.x = wrapped_x * SNAKE_BLOCK;
    pos.y = wrapped_y * SNAKE_BLOCK + GAME_AREA_TOP;
    return pos;
}

static int positions_equal(Position a, Position b) {
    return a.x == b.x && a.y == b.y;
}

static int grid_distance_sq(Position a, Position b) {
    int grid_width = DIS_WIDTH / SNAKE_BLOCK;
    int grid_height = GAME_AREA_HEIGHT / SNAKE_BLOCK;

    int ax = a.x / SNAKE_BLOCK;
    int ay = (a.y - GAME_AREA_TOP) / SNAKE_BLOCK;
    int bx = b.x / SNAKE_BLOCK;
    int by = (b.y - GAME_AREA_TOP) / SNAKE_BLOCK;

    int dx = abs(ax - bx);
    if (dx > grid_width - dx) {
        dx = grid_width - dx;
    }
    int dy = abs(ay - by);
    if (dy > grid_height - dy) {
        dy = grid_height - dy;
    }

    return dx * dx + dy * dy;
}

static int is_safe_move(GameContext* game, int new_dx, int new_dy) {
    Position head = game->snake.segments[game->snake.length - 1];
    Position new_pos = wrap_position(head.x + new_dx, head.y + new_dy);
    int invincible = has_active_powerup(game, POWERUP_INVINCIBLE) || has_active_powerup(game, POWERUP_PATHFIND);

    if (!invincible) {
        for (int i = 0; i < game->snake.length - 1; i++) {
            if (positions_equal(game->snake.segments[i], new_pos)) {
                return 0;
            }
        }
    }

    if (!invincible && game->secondary_food.active && game->secondary_food.type == ITEM_BOMB) {
        if (positions_equal(new_pos, game->secondary_food.pos)) {
            return 0;
        }
    }

    return 1;
}

static int is_opposite_direction(int dx1, int dy1, int dx2, int dy2) {
    return dx1 == -dx2 && dy1 == -dy2;
}

static int select_target(GameContext* game, Position* target_pos) {
    Position food_pos = {0, 0};
    int has_food = 0;
    if (game->food.active && game->food.type != ITEM_BOMB) {
        food_pos = game->food.pos;
        has_food = 1;
    }

    if (game->powerup.active && is_powerup_target_type(game->powerup.type)) {
        Position powerup_pos = game->powerup.pos;

        if (!has_food) {
            *target_pos = powerup_pos;
            return 1;
        }

        int head_distance = grid_distance_sq(game->snake.segments[game->snake.length - 1], powerup_pos);
        int food_distance = grid_distance_sq(game->snake.segments[game->snake.length - 1], food_pos);

        if (head_distance < food_distance) {
            *target_pos = powerup_pos;
            return 1;
        }
    }

    if (has_food) {
        *target_pos = food_pos;
        return 1;
    }

    return 0;
}

void pathfind_update_intelligence(GameContext* game) {
    Position target;
    if (!select_target(game, &target)) {
        return;
    }

    Position head = game->snake.segments[game->snake.length - 1];
    int current_dx = game->snake.dx;
    int current_dy = game->snake.dy;
    int current_safe = is_safe_move(game, current_dx, current_dy);
    int current_dist = current_safe ? grid_distance_sq(head, target) : INT_MAX;

    int best_dx = current_dx;
    int best_dy = current_dy;
    int best_dist = current_dist;
    int found_safe = current_safe && (current_dx != 0 || current_dy != 0);

    int directions[4][2] = {
        {0, -SNAKE_BLOCK},
        {0, SNAKE_BLOCK},
        {-SNAKE_BLOCK, 0},
        {SNAKE_BLOCK, 0}
    };

    for (int i = 0; i < 4; i++) {
        int new_dx = directions[i][0];
        int new_dy = directions[i][1];

        if (is_opposite_direction(new_dx, new_dy, current_dx, current_dy)) {
            continue;
        }

        if (!is_safe_move(game, new_dx, new_dy)) {
            continue;
        }

        Position test_pos = wrap_position(head.x + new_dx, head.y + new_dy);
        int dist = grid_distance_sq(test_pos, target);

        if (dist < best_dist || !found_safe) {
            best_dist = dist;
            best_dx = new_dx;
            best_dy = new_dy;
            found_safe = 1;
        }
    }

    if (found_safe) {
        game->snake.dx = best_dx;
        game->snake.dy = best_dy;
    }
}
