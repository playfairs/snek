#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "../include/game.h"
#include "../include/state.h"
#include "../include/constants.h"
#include "../include/graphics.h"
#include "../include/input.h"
#include "../include/powerups.h"
#include "../include/audio.h"

static void spawn_food(Food* food) {
    int max_x = (DIS_WIDTH - SNAKE_BLOCK) / SNAKE_BLOCK;
    int max_y = (DIS_HEIGHT - GAME_AREA_TOP - SNAKE_BLOCK) / SNAKE_BLOCK;
    
    food->pos.x = (rand() % max_x) * SNAKE_BLOCK;
    food->pos.y = GAME_AREA_TOP + (rand() % max_y) * SNAKE_BLOCK;
    food->active = 1;
}

static int check_self_collision(Snake* snake) {
    Position head = snake->segments[snake->length - 1];
    
    for (int i = 0; i < snake->length - 1; i++) {
        if (snake->segments[i].x == head.x && snake->segments[i].y == head.y) {
            return 1;
        }
    }
    return 0;
}

static int check_food_collision(Snake* snake, Food* food) {
    Position head = snake->segments[snake->length - 1];
    
    if (abs(head.x - food->pos.x) < SNAKE_BLOCK && 
        abs(head.y - food->pos.y) < SNAKE_BLOCK) {
        return 1;
    }
    return 0;
}

static void update_snake_position(Snake* snake, PowerupType active_powerup) {
    Position head = snake->segments[snake->length - 1];
    Position new_head = head;
    
    new_head.x += snake->dx;
    new_head.y += snake->dy;
    
    if (active_powerup != POWERUP_INVINCIBLE) {
        if (new_head.y < GAME_AREA_TOP) {
            new_head.y = DIS_HEIGHT - SNAKE_BLOCK;
        } else if (new_head.y >= DIS_HEIGHT) {
            new_head.y = GAME_AREA_TOP;
        }
        if (new_head.x < 0) {
            new_head.x = DIS_WIDTH - SNAKE_BLOCK;
        } else if (new_head.x >= DIS_WIDTH) {
            new_head.x = 0;
        }
    } else {
        if (new_head.x >= DIS_WIDTH) {
            new_head.x = 0;
        } else if (new_head.x < 0) {
            new_head.x = DIS_WIDTH - SNAKE_BLOCK;
        }
        if (new_head.y >= DIS_HEIGHT) {
            new_head.y = 0;
        } else if (new_head.y < 0) {
            new_head.y = GAME_AREA_TOP;
        }
    }
    
    for (int i = 0; i < snake->length - 1; i++) {
        snake->segments[i] = snake->segments[i + 1];
    }
    snake->segments[snake->length - 1] = new_head;
}

static int get_base_speed(Difficulty difficulty) {
    switch (difficulty) {
        case DIFFICULTY_EASY: return 8;
        case DIFFICULTY_HARD: return 15;
        default: return 10;
    }
}

void init_game(GameContext* game, GameState* state) {
    srand(time(NULL));
    
    game->snake.length = 1;
    game->snake.dx = 0;
    game->snake.dy = 0;
    game->snake.skin = state->settings.current_skin;
    
    int start_x = (DIS_WIDTH / (2 * SNAKE_BLOCK)) * SNAKE_BLOCK;
    int start_y = ((DIS_HEIGHT - GAME_AREA_TOP) / (2 * SNAKE_BLOCK)) * SNAKE_BLOCK + GAME_AREA_TOP;
    
    game->snake.segments[0].x = start_x;
    game->snake.segments[0].y = start_y;
    
    spawn_food(&game->food);
    
    game->powerup.active = 0;
    game->active_powerup = POWERUP_NONE;
    game->powerup_end_time = 0;
    
    game->score = 0;
    game->apples_eaten = 0;
    game->start_time = SDL_GetTicks() / 1000.0;
    game->powerup_spawn_time = game->start_time + 10 + rand() % 10;
    game->current_speed = get_base_speed(state->settings.difficulty);
    game->status = GAME_RUNNING;
    
    state->stats.games_played++;
    save_stats(&state->stats);
}

GameStatus game_loop(GameContext* game, GameState* state, AudioState* audio) {
    SDL_Event event;
    int running = 1;
    
    while (running && game->status == GAME_RUNNING) {
        double current_time = SDL_GetTicks() / 1000.0;
        
        if (!game->powerup.active && current_time > game->powerup_spawn_time) {
            spawn_powerup(&game->powerup);
            game->powerup_spawn_time = current_time + 15 + rand() % 15;
        }
        
        if (game->active_powerup != POWERUP_NONE && current_time > game->powerup_end_time) {
            game->active_powerup = POWERUP_NONE;
        }
        
        if (game->powerup.active && current_time - game->powerup.spawn_time > 10) {
            game->powerup.active = 0;
        }
        
        if (game->active_powerup == POWERUP_SPEED_BOOST) {
            game->current_speed = 15;
        } else if (game->active_powerup == POWERUP_SLOW_MO) {
            game->current_speed = 5;
        } else {
            game->current_speed = get_base_speed(state->settings.difficulty);
        }
        
        while (SDL_PollEvent(&event)) {
            InputAction action = handle_input_snake(&event, game->snake.dx, game->snake.dy);
            
            if (action == INPUT_QUIT) {
                game->status = GAME_QUIT;
                running = 0;
            } else if (action == INPUT_PAUSE) {
                game->status = GAME_OVER;
            } else if (action == INPUT_UP && game->snake.dy == 0) {
                game->snake.dx = 0;
                game->snake.dy = -SNAKE_BLOCK;
            } else if (action == INPUT_DOWN && game->snake.dy == 0) {
                game->snake.dx = 0;
                game->snake.dy = SNAKE_BLOCK;
            } else if (action == INPUT_LEFT && game->snake.dx == 0) {
                game->snake.dx = -SNAKE_BLOCK;
                game->snake.dy = 0;
            } else if (action == INPUT_RIGHT && game->snake.dx == 0) {
                game->snake.dx = SNAKE_BLOCK;
                game->snake.dy = 0;
            }
        }
        
        update_snake_position(&game->snake, game->active_powerup);
        
        if (game->active_powerup != POWERUP_INVINCIBLE && check_self_collision(&game->snake)) {
            game->status = GAME_OVER;
        }
        
        if (check_food_collision(&game->snake, &game->food)) {
            spawn_food(&game->food);
            
            if (game->snake.length < MAX_SNAKE_LENGTH) {
                game->snake.length++;
                game->snake.segments[game->snake.length - 1] = game->snake.segments[game->snake.length - 2];
            }
            
            game->apples_eaten++;
            state->stats.total_apples++;
            
            int points = 10;
            if (game->active_powerup == POWERUP_DOUBLE_POINTS) {
                points *= 2;
            }
            game->score += points;
            
            if (game->score > state->stats.high_score) {
                state->stats.high_score = game->score;
            }
            
            save_stats(&state->stats);
        }
        
        if (game->powerup.active && check_powerup_collision(&game->snake, &game->powerup)) {
            apply_powerup(game, game->powerup.type);
            game->powerup.active = 0;
            state->stats.powerups_collected++;
            save_stats(&state->stats);
            
            if (state->settings.sound_enabled && audio) {
                play_powerup_sound(audio, state->settings.volume);
            }
        }
        
        SDL_SetRenderDrawColor(state->renderer, BLACK_R, BLACK_G, BLACK_B, 255);
        SDL_RenderClear(state->renderer);
        
        draw_grid(state->renderer);
        draw_food(state->renderer, &game->food);
        
        if (game->powerup.active) {
            draw_powerup(state->renderer, &game->powerup);
        }
        
        draw_snake(state->renderer, &game->snake, game->active_powerup);
        
        draw_hud(state->renderer, state->score_font, state->button_font,
                game->score, state->stats.high_score, game->snake.length,
                game->apples_eaten, game->current_speed, game->active_powerup,
                game->powerup_end_time);
        
        SDL_RenderPresent(state->renderer);
        SDL_Delay(1000 / game->current_speed);
    }
    
    return game->status;
}

void cleanup_game(GameContext* game) {
}
