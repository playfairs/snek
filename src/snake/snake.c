#include "snake.h"

void init_snake(Snake* snake, SnakeSkin skin, int start_x, int start_y) {
    snake->length = 1;
    snake->dx = 0;
    snake->dy = 0;
    snake->skin = skin;
    snake->segments[0].x = start_x;
    snake->segments[0].y = start_y;
}
