#include "../registry.h"
#include <stdlib.h>

static int positions_equal(const Position* a, const Position* b) {
    return a->x == b->x && a->y == b->y;
}

static int is_near_head(const Position* head, const Position* target, int range) {
    int dx = head->x - target->x;
    int dy = head->y - target->y;
    return abs(dx) <= range && abs(dy) <= range;
}

static void move_toward_head(Position* target, const Position* head) {
    if (target->x < head->x) target->x += SNAKE_BLOCK;
    else if (target->x > head->x) target->x -= SNAKE_BLOCK;

    if (target->y < head->y) target->y += SNAKE_BLOCK;
    else if (target->y > head->y) target->y -= SNAKE_BLOCK;
}

static void clamp_position(Position* pos) {
    if (pos->x < 0) pos->x = 0;
    if (pos->x >= DIS_WIDTH) pos->x = DIS_WIDTH - SNAKE_BLOCK;
    if (pos->y < GAME_AREA_TOP) pos->y = GAME_AREA_TOP;
    if (pos->y >= GAME_AREA_BOTTOM) pos->y = GAME_AREA_BOTTOM - SNAKE_BLOCK;
}

static int has_active_powerup(GameContext* game, PowerupType type) {
    for (int i = 0; i < game->powerup_count; i++) {
        if (game->active_powerups[i].type == type) {
            return 1;
        }
    }
    return 0;
}

static void consume_food(GameContext* game, Food* food) {
    if (!food->active) return;
    if (food->type == ITEM_BOMB) {
        if (has_active_powerup(game, POWERUP_INVINCIBLE)) {
            food->active = 0;
            return;
        }
        game->status = GAME_OVER;
        return;
    }

    food->active = 0;
    int points = (food->type == ITEM_BANANA) ? 15 : 10;
    if (game->powerup_count > 0) {
        for (int i = 0; i < game->powerup_count; i++) {
            if (game->active_powerups[i].type == POWERUP_DOUBLE_POINTS) {
                points *= 2;
                break;
            }
        }
    }

    int previous_tail_index = game->snake.length - 1;
    if (previous_tail_index < 0) {
        previous_tail_index = 0;
    }
    game->snake.length++;
    game->snake.segments[game->snake.length - 1] = game->snake.segments[previous_tail_index];
    game->score += points;
    game->apples_eaten++;
}

static void consume_powerup_item(GameContext* game) {
    if (!game->powerup.active) return;
    game->powerup.active = 0;
    apply_powerup(game, game->powerup.type);
}

void magnet_apply(GameContext* game) {
    (void)game;
}

void magnet_update(GameContext* game) {
    Position head = game->snake.segments[game->snake.length - 1];
    int range = 5 * SNAKE_BLOCK;

    if (game->food.active && is_near_head(&head, &game->food.pos, range)) {
        move_toward_head(&game->food.pos, &head);
        clamp_position(&game->food.pos);
        if (positions_equal(&head, &game->food.pos)) {
            consume_food(game, &game->food);
        }
    }

    if (game->secondary_food.active && is_near_head(&head, &game->secondary_food.pos, range)) {
        move_toward_head(&game->secondary_food.pos, &head);
        clamp_position(&game->secondary_food.pos);
        if (positions_equal(&head, &game->secondary_food.pos)) {
            consume_food(game, &game->secondary_food);
        }
    }

    if (game->powerup.active && is_near_head(&head, &game->powerup.pos, range)) {
        move_toward_head(&game->powerup.pos, &head);
        clamp_position(&game->powerup.pos);
        if (positions_equal(&head, &game->powerup.pos)) {
            consume_powerup_item(game);
        }
    }
}

void magnet_remove(GameContext* game) {
    (void)game;
}
