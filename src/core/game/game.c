#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "core/game/game.h"
#include "core/game/state.h"
#include "core/game/constants.h"
#include "core/graphics/graphics.h"
#include "core/input/input.h"
#include "powerups/registry.h"
#include "items/registry.h"
#include "core/audio/audio.h"
#include "snake/snake.h"

static ItemType random_fruit_type(void) {
    int roll = rand() % 100;
    return (roll < 55) ? ITEM_APPLE : ITEM_BANANA;
}

static void init_food(Food* food, ItemType type) {
    int max_x = DIS_WIDTH / SNAKE_BLOCK;
    int max_y = GAME_AREA_HEIGHT / SNAKE_BLOCK;

    food->pos.x = (rand() % max_x) * SNAKE_BLOCK;
    food->pos.y = GAME_AREA_TOP + (rand() % max_y) * SNAKE_BLOCK;
    food->type = type;
    food->active = 1;
    food->spawn_time = SDL_GetTicks() / 1000.0;
    food->lifetime = (type == ITEM_BOMB) ? 5.0 + (rand() % 6) : 0.0;
}

static void spawn_bomb(GameContext* game) {
    init_food(&game->secondary_food, ITEM_BOMB);
    while (game->secondary_food.pos.x == game->food.pos.x &&
           game->secondary_food.pos.y == game->food.pos.y) {
        init_food(&game->secondary_food, ITEM_BOMB);
    }
}

static void maybe_spawn_bomb(GameContext* game) {
    if (!game->secondary_food.active && random_item_type() == ITEM_BOMB) {
        spawn_bomb(game);
    }
}

static void spawn_primary_food(GameContext* game) {
    init_food(&game->food, random_fruit_type());
    maybe_spawn_bomb(game);
}

static int has_active_powerup(GameContext* game, PowerupType type) {
    for (int i = 0; i < game->powerup_count; i++) {
        if (game->active_powerups[i].type == type) {
            return 1;
        }
    }
    return 0;
}

/* Snake helper implementations moved to `src/snake/` modules. */

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

    game->powerup.active = 0;
    game->powerup.spawn_time = 0.0;
    game->powerup_count = 0;
    game->secondary_food.active = 0;
    game->mode = state->settings.mode;

    double current_time = SDL_GetTicks() / 1000.0;
    game->powerup_spawn_time = current_time + POWERUP_INITIAL_SPAWN_DELAY + rand() % POWERUP_SPAWN_DELAY_VARIANCE;

    spawn_primary_food(game);
    game->score = 0;
    game->apples_eaten = 0;
    game->start_time = current_time;
    game->mode_end_time = (game->mode == MODE_TIME_ATTACK) ? current_time + TIME_ATTACK_DURATION : 0;

    double spawn_delay = POWERUP_INITIAL_SPAWN_DELAY + rand() % POWERUP_SPAWN_DELAY_VARIANCE;
    if (game->mode == MODE_CHALLENGE) {
        spawn_delay = fmax(4.0, spawn_delay - 3.0);
    } else if (game->mode == MODE_TIME_ATTACK) {
        spawn_delay = fmax(5.0, spawn_delay - 2.0);
    }
    game->powerup_spawn_time = game->start_time + spawn_delay;

    game->base_speed = get_base_speed(state->settings.difficulty) + (game->mode == MODE_CHALLENGE ? 2 : 0);
    game->current_speed = game->base_speed;
    game->status = GAME_RUNNING;

    state->stats.games_played++;
    save_stats(&state->stats);
}

GameStatus game_loop(GameContext* game, GameState* state, AudioState* audio) {
    SDL_Event event;
    int running = 1;
    int paused = 0;
    double pause_start_time = 0.0;

    while (running && game->status == GAME_RUNNING) {
        double current_time = SDL_GetTicks() / 1000.0;

        while (SDL_PollEvent(&event)) {
            InputAction action = handle_input_snake(&event, game->snake.dx, game->snake.dy);

            if (action == INPUT_QUIT) {
                game->status = GAME_QUIT;
                running = 0;
            } else if (action == INPUT_PAUSE) {
                if (!paused) {
                    paused = 1;
                    pause_start_time = current_time;
                } else {
                    double resume_time = SDL_GetTicks() / 1000.0;
                    double pause_duration = resume_time - pause_start_time;
                    for (int i = 0; i < game->powerup_count; i++) {
                        game->active_powerups[i].end_time += pause_duration;
                    }
                    if (game->mode == MODE_TIME_ATTACK) {
                        game->mode_end_time += pause_duration;
                    }
                    game->powerup_spawn_time += pause_duration;
                    paused = 0;
                }
            } else if (!paused) {
                if (action == INPUT_UP && game->snake.dy == 0) {
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
        }

        if (!paused) {
            if (!game->powerup.active && current_time > game->powerup_spawn_time) {
                spawn_powerup(&game->powerup);
                game->powerup_spawn_time = current_time + POWERUP_INITIAL_SPAWN_DELAY + rand() % POWERUP_SPAWN_DELAY_VARIANCE;
            }
            update_active_powerup(game, current_time);

            if (game->powerup.active && current_time - game->powerup.spawn_time > POWERUP_ITEM_LIFETIME) {
                game->powerup.active = 0;
            }

            if (game->secondary_food.active &&
                current_time - game->secondary_food.spawn_time > game->secondary_food.lifetime) {
                game->secondary_food.active = 0;
            }

            if (!game->food.active) {
                spawn_primary_food(game);
            }

            if (game->mode == MODE_TIME_ATTACK && current_time >= game->mode_end_time) {
                game->status = GAME_OVER;
            }

            for (int i = 0; i < game->powerup_count; i++) {
                update_powerup_effect(game, game->active_powerups[i].type);
            }

            int bypass_collision = 0;
            for (int i = 0; i < game->powerup_count; i++) {
                PowerupType type = game->active_powerups[i].type;
                if (type == POWERUP_INVINCIBLE || type == POWERUP_PATHFIND) {
                    bypass_collision = 1;
                    break;
                }
            }
            update_snake_position(&game->snake);

            if (!bypass_collision && check_self_collision(&game->snake)) {
                game->status = GAME_OVER;
            }

            if (check_food_collision(&game->snake, &game->food)) {
                ItemType item_type = game->food.type;
                game->snake.length++;
                game->snake.segments[game->snake.length - 1] = game->snake.segments[game->snake.length - 2];

                game->apples_eaten++;
                state->stats.total_apples++;

                int points = (item_type == ITEM_BANANA) ? 15 : 10;
                for (int i = 0; i < game->powerup_count; i++) {
                    if (game->active_powerups[i].type == POWERUP_DOUBLE_POINTS) {
                        points *= 2;
                        break;
                    }
                }
                game->score += points;

                if (game->score > state->stats.high_score) {
                    state->stats.high_score = game->score;
                }

                save_stats(&state->stats);
                spawn_primary_food(game);
            } else if (game->secondary_food.active &&
                       check_food_collision(&game->snake, &game->secondary_food)) {
                if (has_active_powerup(game, POWERUP_INVINCIBLE)) {
                    game->secondary_food.active = 0;
                } else {
                    game->status = GAME_OVER;
                    if (state->settings.sound_enabled && audio) {
                        play_explosion_sound(audio, state->settings.volume);
                    }
                }
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
        }

        SDL_SetRenderDrawColor(state->renderer, BLACK_R, BLACK_G, BLACK_B, 255);
        SDL_RenderClear(state->renderer);

        draw_grid(state->renderer);
        draw_food(state->renderer, &game->food);
        if (game->secondary_food.active) {
            draw_food(state->renderer, &game->secondary_food);
        }

        if (game->powerup.active) {
            draw_powerup(state->renderer, &game->powerup);
        }

        int glow_active = has_active_powerup(game, POWERUP_INVINCIBLE) || has_active_powerup(game, POWERUP_PATHFIND);
        draw_snake(state->renderer, &game->snake, glow_active);

        double time_left = 0.0;
        if (game->mode == MODE_TIME_ATTACK) {
            time_left = game->mode_end_time - current_time;
            if (time_left < 0.0) {
                time_left = 0.0;
            }
        }

        draw_hud(state->renderer, state->score_font, state->button_font,
            game->score, state->stats.high_score, game->snake.length,
            game->apples_eaten, game->current_speed, game->active_powerups,
            game->powerup_count, game->mode, time_left);

        if (paused) {
            SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 150);
            SDL_Rect overlay = {0, GAME_AREA_TOP, DIS_WIDTH, GAME_AREA_HEIGHT};
            SDL_RenderFillRect(state->renderer, &overlay);

            SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
            SDL_Surface* paused_surf = TTF_RenderText_Blended(state->large_font, "PAUSED", white);
            SDL_Texture* paused_tex = SDL_CreateTextureFromSurface(state->renderer, paused_surf);
            SDL_Rect paused_rect = {(DIS_WIDTH - paused_surf->w) / 2, GAME_AREA_TOP + GAME_AREA_HEIGHT / 2 - 40, paused_surf->w, paused_surf->h};
            SDL_RenderCopy(state->renderer, paused_tex, NULL, &paused_rect);
            SDL_FreeSurface(paused_surf);
            SDL_DestroyTexture(paused_tex);
        }

        SDL_RenderPresent(state->renderer);

        if (paused) {
            SDL_Delay(1000 / 60);
        } else {
            SDL_Delay(1000 / game->current_speed);
        }
    }

    return game->status;
}

void cleanup_game(GameContext* game) {
    (void)game;
}
