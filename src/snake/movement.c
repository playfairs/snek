#include "snake.h"

void update_snake_direction(Snake* snake, InputAction action) {
    if (action == INPUT_UP && snake->dy == 0) {
        snake->dx = 0;
        snake->dy = -SNAKE_BLOCK;
    } else if (action == INPUT_DOWN && snake->dy == 0) {
        snake->dx = 0;
        snake->dy = SNAKE_BLOCK;
    } else if (action == INPUT_LEFT && snake->dx == 0) {
        snake->dx = -SNAKE_BLOCK;
        snake->dy = 0;
    } else if (action == INPUT_RIGHT && snake->dx == 0) {
        snake->dx = SNAKE_BLOCK;
        snake->dy = 0;
    }
}

void update_snake_position(Snake* snake) {
    Position head = snake->segments[snake->length - 1];
    Position new_head = head;

    new_head.x += snake->dx;
    new_head.y += snake->dy;

    if (new_head.y < GAME_AREA_TOP) {
        new_head.y = GAME_AREA_BOTTOM - SNAKE_BLOCK;
    } else if (new_head.y >= GAME_AREA_BOTTOM) {
        new_head.y = GAME_AREA_TOP;
    }
    if (new_head.x < 0) {
        new_head.x = DIS_WIDTH - SNAKE_BLOCK;
    } else if (new_head.x >= DIS_WIDTH) {
        new_head.x = 0;
    }

    for (int i = 0; i < snake->length - 1; i++) {
        snake->segments[i] = snake->segments[i + 1];
    }
    snake->segments[snake->length - 1] = new_head;
}
