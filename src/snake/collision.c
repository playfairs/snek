#include "snake.h"

int check_self_collision(const Snake* snake) {
    Position head = snake->segments[snake->length - 1];
    for (int i = 0; i < snake->length - 1; i++) {
        if (snake->segments[i].x == head.x && snake->segments[i].y == head.y) {
            return 1;
        }
    }
    return 0;
}

int check_food_collision(const Snake* snake, const Food* food) {
    if (!food->active) {
        return 0;
    }

    Position head = snake->segments[snake->length - 1];
    return abs(head.x - food->pos.x) < SNAKE_BLOCK && abs(head.y - food->pos.y) < SNAKE_BLOCK;
}

int check_powerup_collision(const Snake* snake, const Powerup* powerup) {
    if (!powerup->active) {
        return 0;
    }

    Position head = snake->segments[snake->length - 1];
    return abs(head.x - powerup->pos.x) < SNAKE_BLOCK && abs(head.y - powerup->pos.y) < SNAKE_BLOCK;
}
