#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "state.h"
#include "game.h"

void draw_cube(SDL_Renderer* renderer, int x, int y, int size, Color color, int is_head);
void draw_grid(SDL_Renderer* renderer);
void draw_snake(SDL_Renderer* renderer, Snake* snake, PowerupType active_powerup);
void draw_food(SDL_Renderer* renderer, Food* food);
void draw_powerup(SDL_Renderer* renderer, Powerup* powerup);
void draw_hud(SDL_Renderer* renderer, TTF_Font* score_font, TTF_Font* button_font, 
              int score, int high_score, int snake_length, int apples_collected, 
              int current_speed, PowerupType active_powerup, double powerup_end_time,
              GameMode mode, double time_left);
void draw_main_menu(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* button_font);
void draw_settings_menu(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* score_font, 
                        TTF_Font* button_font, GameState* state, int current_option);
void draw_game_over(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* score_font, 
                   TTF_Font* button_font, int score);

#endif
