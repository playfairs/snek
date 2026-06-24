#include "powerup.h"
#include <math.h>

static int distance_to_food(Position pos, Position food) {
    int dx = pos.x - food.x;
    int dy = pos.y - food.y;
    return dx * dx + dy * dy;
}

static int is_safe_move(GameContext* game, int new_dx, int new_dy) {
    Position head = game->snake.segments[game->snake.length - 1];
    Position new_pos = {head.x + new_dx, head.y + new_dy};
    
    for (int i = 0; i < game->snake.length - 1; i++) {
        if (game->snake.segments[i].x == new_pos.x && 
            game->snake.segments[i].y == new_pos.y) {
            return 0;
        }
    }
    return 1;
}

void pathfind_apply(GameContext* game) {
    (void)game;
}

static Food* get_pathfind_target(GameContext* game) {
    if (game->food.active && game->food.type != ITEM_BOMB) {
        return &game->food;
    }
    if (game->secondary_food.active) {
        return &game->secondary_food;
    }
    return NULL;
}

void pathfind_update(GameContext* game) {
    Food* target = get_pathfind_target(game);
    if (!target) {
        return;
    }

    Position head = game->snake.segments[game->snake.length - 1];
    
    int best_dx = game->snake.dx;
    int best_dy = game->snake.dy;
    int best_dist = distance_to_food(head, target->pos);
    
    int directions[4][2] = {{0, -SNAKE_BLOCK}, {0, SNAKE_BLOCK}, {-SNAKE_BLOCK, 0}, {SNAKE_BLOCK, 0}};
    
    for (int i = 0; i < 4; i++) {
        int new_dx = directions[i][0];
        int new_dy = directions[i][1];
        
        if (new_dx == -game->snake.dx && new_dy == -game->snake.dy) {
            continue;
        }
        
        if (is_safe_move(game, new_dx, new_dy)) {
            Position test_pos = {head.x + new_dx, head.y + new_dy};
            int dist = distance_to_food(test_pos, game->food.pos);
            
            if (dist < best_dist) {
                best_dist = dist;
                best_dx = new_dx;
                best_dy = new_dy;
            }
        }
    }
    
    game->snake.dx = best_dx;
    game->snake.dy = best_dy;
}

void pathfind_remove(GameContext* game) {
    (void)game;
}
