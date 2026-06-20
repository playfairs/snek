#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../include/graphics.h"
#include "../include/state.h"
#include "../include/constants.h"

static Color get_skin_color(SnakeSkin skin, int is_head) {
    switch (skin) {
        case SKIN_BLUE:
            return is_head ? (Color){BLUE_R, BLUE_G, BLUE_B} : (Color){70, 120, 200};
        case SKIN_RED:
            return is_head ? (Color){RED_R, RED_G, RED_B} : (Color){200, 60, 60};
        case SKIN_RAINBOW:
            return is_head ? (Color){ACCENT_R, ACCENT_G, ACCENT_B} : (Color){ACCENT2_R, ACCENT2_G, ACCENT2_B};
        default:
            return is_head ? (Color){GREEN_R, GREEN_G, GREEN_B} : (Color){60, 160, 100};
    }
}

static Color get_powerup_color(PowerupType type) {
    switch (type) {
        case POWERUP_SPEED_BOOST: return (Color){100, 200, 255};
        case POWERUP_SLOW_MO: return (Color){180, 100, 255};
        case POWERUP_DOUBLE_POINTS: return (Color){255, 200, 80};
        case POWERUP_INVINCIBLE: return (Color){255, 100, 200};
        default: return (Color){WHITE_R, WHITE_G, WHITE_B};
    }
}

static Color apply_powerup_glow(Color base, PowerupType active_powerup) {
    if (active_powerup != POWERUP_INVINCIBLE) {
        return base;
    }
    
    double pulse = (SDL_GetTicks() % 1000) / 1000.0;
    int highlight = 55 + (int)(100 * (0.5 + 0.5 * sin(pulse * 2 * M_PI)));
    
    return (Color){
        base.r + highlight > 255 ? 255 : base.r + highlight,
        base.g + highlight > 255 ? 255 : base.g + highlight,
        base.b + highlight > 255 ? 255 : base.b + highlight
    };
}

void draw_cube(SDL_Renderer* renderer, int x, int y, int size, Color color, int is_head) {
    SDL_Rect rect = {x, y, size, size};
    
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &rect);
    
    Color highlight = {
        color.r + (is_head ? 80 : 50) > 255 ? 255 : color.r + (is_head ? 80 : 50),
        color.g + (is_head ? 80 : 50) > 255 ? 255 : color.g + (is_head ? 80 : 50),
        color.b + (is_head ? 40 : 30) > 255 ? 255 : color.b + (is_head ? 40 : 30)
    };
    
    Color shadow = {
        color.r - (is_head ? 60 : 30) < 0 ? 0 : color.r - (is_head ? 60 : 30),
        color.g - (is_head ? 40 : 20) < 0 ? 0 : color.g - (is_head ? 40 : 20),
        color.b - (is_head ? 20 : 10) < 0 ? 0 : color.b - (is_head ? 20 : 10)
    };
    
    int line_width = is_head ? 2 : 1;
    
    SDL_SetRenderDrawColor(renderer, highlight.r, highlight.g, highlight.b, 255);
    SDL_RenderDrawLine(renderer, x, y, x + size - 1, y);
    SDL_RenderDrawLine(renderer, x, y, x, y + size - 1);
    
    SDL_SetRenderDrawColor(renderer, shadow.r, shadow.g, shadow.b, 255);
    SDL_RenderDrawLine(renderer, x + 1, y + size - 1, x + size - 1, y + size - 1);
    SDL_RenderDrawLine(renderer, x + size - 1, y + 1, x + size - 1, y + size - 1);
    
    if (is_head) {
        Color inner_highlight = {
            color.r + 100 > 255 ? 255 : color.r + 100,
            color.g + 100 > 255 ? 255 : color.g + 100,
            color.b + 60 > 255 ? 255 : color.b + 60
        };
        SDL_Rect inner_rect = {x + 2, y + 2, size / 3, size / 3};
        SDL_SetRenderDrawColor(renderer, inner_highlight.r, inner_highlight.g, inner_highlight.b, 255);
        SDL_RenderFillRect(renderer, &inner_rect);
    }
}

void draw_grid(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, GRID_COLOR_R, GRID_COLOR_G, GRID_COLOR_B, 255);
    
    for (int x = 0; x < DIS_WIDTH; x += SNAKE_BLOCK) {
        SDL_RenderDrawLine(renderer, x, GAME_AREA_TOP, x, DIS_HEIGHT);
    }
    
    for (int y = GAME_AREA_TOP; y < DIS_HEIGHT; y += SNAKE_BLOCK) {
        SDL_RenderDrawLine(renderer, 0, y, DIS_WIDTH, y);
    }
}

void draw_snake(SDL_Renderer* renderer, Snake* snake, PowerupType active_powerup) {
    for (int i = 0; i < snake->length - 1; i++) {
        Color base_color = get_skin_color(snake->skin, 0);
        
        if (snake->skin == SKIN_RAINBOW) {
            double time = SDL_GetTicks() / 200.0;
            double phase = (i * 0.3) + (snake->segments[i].x * 0.01) + (snake->segments[i].y * 0.01);
            
            int r = (int)(128 + 127 * sin(time + phase));
            int g = (int)(128 + 127 * sin(time + phase + 2.094));
            int b = (int)(128 + 127 * sin(time + phase + 4.188));
            
            double shimmer = 0.5 + 0.5 * sin(time * 2 + phase);
            r = (int)(r * (0.7 + 0.3 * shimmer));
            g = (int)(g * (0.7 + 0.3 * shimmer));
            b = (int)(b * (0.7 + 0.3 * shimmer));
            
            base_color = (Color){r, g, b};
        }
        
        Color color = apply_powerup_glow(base_color, active_powerup);
        draw_cube(renderer, snake->segments[i].x, snake->segments[i].y, SNAKE_BLOCK, color, 0);
    }
    
    if (snake->length > 0) {
        Position head = snake->segments[snake->length - 1];
        Color base_color = get_skin_color(snake->skin, 1);
        
        if (snake->skin == SKIN_RAINBOW) {
            double time = SDL_GetTicks() / 200.0;
            double phase = (snake->length * 0.3) + (head.x * 0.01) + (head.y * 0.01);
            
            int r = (int)(128 + 127 * sin(time + phase));
            int g = (int)(128 + 127 * sin(time + phase + 2.094));
            int b = (int)(128 + 127 * sin(time + phase + 4.188));
            
            double shimmer = 0.5 + 0.5 * sin(time * 2 + phase);
            r = (int)(r * (0.8 + 0.2 * shimmer));
            g = (int)(g * (0.8 + 0.2 * shimmer));
            b = (int)(b * (0.8 + 0.2 * shimmer));
            
            base_color = (Color){r, g, b};
        }
        
        Color color = apply_powerup_glow(base_color, active_powerup);
        draw_cube(renderer, head.x, head.y, SNAKE_BLOCK, color, 1);
        
        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        SDL_Rect head_rect = {head.x, head.y, SNAKE_BLOCK, SNAKE_BLOCK};
        SDL_RenderDrawRect(renderer, &head_rect);
    }
}

void draw_food(SDL_Renderer* renderer, Food* food) {
    if (!food->active) return;
    
    double pulse = (SDL_GetTicks() % 1000) / 1000.0;
    int pulse_size = (int)(2 * (0.5 + 0.5 * sin(pulse * 2 * M_PI)));
    
    SDL_Rect food_rect = {food->pos.x, food->pos.y, SNAKE_BLOCK, SNAKE_BLOCK};
    SDL_SetRenderDrawColor(renderer, RED_R, RED_G, RED_B, 255);
    SDL_RenderFillRect(renderer, &food_rect);
    
    Color highlight = {
        RED_R + 50 > 255 ? 255 : RED_R + 50,
        RED_G - 50 < 0 ? 0 : RED_G - 50,
        RED_B - 50 < 0 ? 0 : RED_B - 50
    };
    
    int inner_size = SNAKE_BLOCK - 4 - pulse_size;
    int inner_offset = (SNAKE_BLOCK - inner_size) / 2;
    SDL_Rect inner_rect = {food->pos.x + inner_offset, food->pos.y + inner_offset, inner_size, inner_size};
    SDL_SetRenderDrawColor(renderer, highlight.r, highlight.g, highlight.b, 255);
    SDL_RenderFillRect(renderer, &inner_rect);
}

void draw_powerup(SDL_Renderer* renderer, Powerup* powerup) {
    if (!powerup->active) return;
    
    Color color = get_powerup_color(powerup->type);
    
    double pulse = (SDL_GetTicks() % 1000) / 1000.0;
    int pulse_size = (int)(3 * (0.5 + 0.5 * sin(pulse * 2 * M_PI)));
    
    Color glow_color = {
        color.r + 50 > 255 ? 255 : color.r + 50,
        color.g + 50 > 255 ? 255 : color.g + 50,
        color.b + 50 > 255 ? 255 : color.b + 50
    };
    
    int glow_size = SNAKE_BLOCK + 4 + pulse_size;
    int glow_offset = (glow_size - SNAKE_BLOCK) / 2;
    
    SDL_Rect glow_rect = {powerup->pos.x - glow_offset, powerup->pos.y - glow_offset, glow_size, glow_size};
    SDL_SetRenderDrawColor(renderer, glow_color.r, glow_color.g, glow_color.b, 255);
    SDL_RenderDrawRect(renderer, &glow_rect);
    
    SDL_Rect powerup_rect = {powerup->pos.x, powerup->pos.y, SNAKE_BLOCK, SNAKE_BLOCK};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &powerup_rect);
    
    SDL_SetRenderDrawColor(renderer, WHITE_R, WHITE_G, WHITE_B, 255);
    SDL_RenderDrawRect(renderer, &powerup_rect);
}

void draw_hud(SDL_Renderer* renderer, TTF_Font* score_font, TTF_Font* button_font,
              int score, int high_score, int snake_length, int apples_collected,
              int current_speed, PowerupType active_powerup, double powerup_end_time) {
    SDL_Rect header_rect = {0, 0, DIS_WIDTH, HEADER_HEIGHT};
    SDL_SetRenderDrawColor(renderer, BLACK_R, BLACK_G, BLACK_B, 255);
    SDL_RenderFillRect(renderer, &header_rect);
    
    for (int i = 0; i < HEADER_HEIGHT; i++) {
        int alpha = (int)(30 * ((double)i / HEADER_HEIGHT));
        SDL_SetRenderDrawColor(renderer, 40 + alpha, 40 + alpha, 50 + alpha, 255);
        SDL_RenderDrawLine(renderer, 0, i, DIS_WIDTH, i);
    }
    
    SDL_SetRenderDrawColor(renderer, ACCENT_R, ACCENT_G, ACCENT_B, 255);
    SDL_RenderDrawLine(renderer, 0, 0, DIS_WIDTH, 0);
    SDL_RenderDrawLine(renderer, 0, HEADER_HEIGHT - 1, DIS_WIDTH, HEADER_HEIGHT - 1);
    
    char score_text[32];
    snprintf(score_text, sizeof(score_text), "SCORE: %d", score);
    
    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    SDL_Color accent = {ACCENT_R, ACCENT_G, ACCENT_B, 255};
    
    SDL_Surface* score_surf = TTF_RenderText_Blended(score_font, score_text, white);
    SDL_Texture* score_tex = SDL_CreateTextureFromSurface(renderer, score_surf);
    SDL_Rect score_rect = {20, 25, score_surf->w, score_surf->h};
    SDL_RenderCopy(renderer, score_tex, NULL, &score_rect);
    SDL_FreeSurface(score_surf);
    SDL_DestroyTexture(score_tex);
    
    if (high_score >= 0) {
        char high_text[32];
        snprintf(high_text, sizeof(high_text), "HIGH: %d", high_score);
        
        SDL_Surface* high_surf = TTF_RenderText_Blended(score_font, high_text, white);
        SDL_Texture* high_tex = SDL_CreateTextureFromSurface(renderer, high_surf);
        SDL_Rect high_rect = {DIS_WIDTH - high_surf->w - 20, 25, high_surf->w, high_surf->h};
        SDL_RenderCopy(renderer, high_tex, NULL, &high_rect);
        SDL_FreeSurface(high_surf);
        SDL_DestroyTexture(high_tex);
    }
    
    char length_text[32];
    snprintf(length_text, sizeof(length_text), "LEN: %d", snake_length);
    
    SDL_Surface* length_surf = TTF_RenderText_Blended(score_font, length_text, white);
    SDL_Texture* length_tex = SDL_CreateTextureFromSurface(renderer, length_surf);
    SDL_Rect length_rect = {150, 25, length_surf->w, length_surf->h};
    SDL_RenderCopy(renderer, length_tex, NULL, &length_rect);
    SDL_FreeSurface(length_surf);
    SDL_DestroyTexture(length_tex);
    
    char apples_text[32];
    snprintf(apples_text, sizeof(apples_text), "APPLES: %d", apples_collected);
    
    SDL_Surface* apples_surf = TTF_RenderText_Blended(score_font, apples_text, white);
    SDL_Texture* apples_tex = SDL_CreateTextureFromSurface(renderer, apples_surf);
    SDL_Rect apples_rect = {280, 25, apples_surf->w, apples_surf->h};
    SDL_RenderCopy(renderer, apples_tex, NULL, &apples_rect);
    SDL_FreeSurface(apples_surf);
    SDL_DestroyTexture(apples_tex);
    
    char speed_text[32];
    snprintf(speed_text, sizeof(speed_text), "SPEED: %d", current_speed);
    
    SDL_Surface* speed_surf = TTF_RenderText_Blended(score_font, speed_text, white);
    SDL_Texture* speed_tex = SDL_CreateTextureFromSurface(renderer, speed_surf);
    SDL_Rect speed_rect = {410, 25, speed_surf->w, speed_surf->h};
    SDL_RenderCopy(renderer, speed_tex, NULL, &speed_rect);
    SDL_FreeSurface(speed_surf);
    SDL_DestroyTexture(speed_tex);
    
    int current_x = 540;
    
    if (active_powerup != POWERUP_NONE) {
        double current_time = SDL_GetTicks() / 1000.0;
        double time_left = powerup_end_time - current_time;
        
        if (time_left > 0) {
            const char* powerup_names[] = {"SPEED BOOST", "SLOW MOTION", "2X POINTS", "INVINCIBLE"};
            const char* name = powerup_names[active_powerup];
            
            char time_text[32];
            snprintf(time_text, sizeof(time_text), "%.1fs", time_left);
            
            Color pcolor = get_powerup_color(active_powerup);
            
            SDL_Surface* name_surf = TTF_RenderText_Blended(button_font, name, white);
            SDL_Texture* name_tex = SDL_CreateTextureFromSurface(renderer, name_surf);
            SDL_Rect name_rect = {current_x, 15, name_surf->w, name_surf->h};
            SDL_RenderCopy(renderer, name_tex, NULL, &name_rect);
            SDL_FreeSurface(name_surf);
            SDL_DestroyTexture(name_tex);
            
            SDL_Surface* time_surf = TTF_RenderText_Blended(button_font, time_text, white);
            SDL_Texture* time_tex = SDL_CreateTextureFromSurface(renderer, time_surf);
            SDL_Rect time_rect = {current_x, 35, time_surf->w, time_surf->h};
            SDL_RenderCopy(renderer, time_tex, NULL, &time_rect);
            SDL_FreeSurface(time_surf);
            SDL_DestroyTexture(time_tex);
        }
    }
    
    SDL_SetRenderDrawColor(renderer, DARK_GRAY_R, DARK_GRAY_G, DARK_GRAY_B, 255);
    SDL_RenderDrawLine(renderer, DIS_WIDTH / 2, 8, DIS_WIDTH / 2, HEADER_HEIGHT - 8);
}

void draw_main_menu(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* button_font) {
    for (int i = 0; i < DIS_HEIGHT; i++) {
        int alpha = (int)(25 * ((double)i / DIS_HEIGHT));
        SDL_SetRenderDrawColor(renderer, 15 + alpha, 15 + alpha, 20 + alpha, 255);
        SDL_RenderDrawLine(renderer, 0, i, DIS_WIDTH, i);
    }
    
    double pulse = (SDL_GetTicks() % 2000) / 2000.0;
    int title_color_r = BLUE_R + (int)(50 * sin(pulse * 2 * M_PI));
    int title_color_g = BLUE_G + (int)(50 * sin(pulse * 2 * M_PI));
    int title_color_b = BLUE_B + (int)(50 * sin(pulse * 2 * M_PI));
    
    SDL_Color title_color = {title_color_r, title_color_g, title_color_b, 255};
    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    
    SDL_Surface* title_surf = TTF_RenderText_Blended(large_font, "SNAKE GAME", white);
    SDL_Texture* title_tex = SDL_CreateTextureFromSurface(renderer, title_surf);
    SDL_Rect title_rect = {(DIS_WIDTH - title_surf->w) / 2, DIS_HEIGHT / 3, title_surf->w, title_surf->h};
    SDL_RenderCopy(renderer, title_tex, NULL, &title_rect);
    SDL_FreeSurface(title_surf);
    SDL_DestroyTexture(title_tex);
}

void draw_settings_menu(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* score_font,
                        TTF_Font* button_font, GameState* state, int current_option) {
    for (int i = 0; i < DIS_HEIGHT; i++) {
        int alpha = (int)(20 * ((double)i / DIS_HEIGHT));
        SDL_SetRenderDrawColor(renderer, 15 + alpha, 15 + alpha, 20 + alpha, 255);
        SDL_RenderDrawLine(renderer, 0, i, DIS_WIDTH, i);
    }
    
    SDL_Color accent = {ACCENT_R, ACCENT_G, ACCENT_B, 255};
    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    SDL_Color light_gray = {LIGHT_GRAY_R, LIGHT_GRAY_G, LIGHT_GRAY_B, 255};
    
    SDL_Surface* title_surf = TTF_RenderText_Blended(large_font, "SETTINGS", accent);
    SDL_Texture* title_tex = SDL_CreateTextureFromSurface(renderer, title_surf);
    SDL_Rect title_rect = {(DIS_WIDTH - title_surf->w) / 2, DIS_HEIGHT / 4, title_surf->w, title_surf->h};
    SDL_RenderCopy(renderer, title_tex, NULL, &title_rect);
    SDL_FreeSurface(title_surf);
    SDL_DestroyTexture(title_tex);
}

void draw_game_over(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* score_font,
                   TTF_Font* button_font, int score) {
    for (int i = 0; i < DIS_HEIGHT; i++) {
        int alpha = (int)(20 * ((double)i / DIS_HEIGHT));
        SDL_SetRenderDrawColor(renderer, 15 + alpha, 15 + alpha, 20 + alpha, 255);
        SDL_RenderDrawLine(renderer, 0, i, DIS_WIDTH, i);
    }
    
    SDL_Color red = {RED_R, RED_G, RED_B, 255};
    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    
    SDL_Surface* title_surf = TTF_RenderText_Blended(large_font, "GAME OVER", white);
    SDL_Texture* title_tex = SDL_CreateTextureFromSurface(renderer, title_surf);
    SDL_Rect title_rect = {(DIS_WIDTH - title_surf->w) / 2, DIS_HEIGHT / 3, title_surf->w, title_surf->h};
    SDL_RenderCopy(renderer, title_tex, NULL, &title_rect);
    SDL_FreeSurface(title_surf);
    SDL_DestroyTexture(title_tex);
    
    char score_text[32];
    snprintf(score_text, sizeof(score_text), "Final Score: %d", score);
    
    SDL_Surface* score_surf = TTF_RenderText_Blended(score_font, score_text, white);
    SDL_Texture* score_tex = SDL_CreateTextureFromSurface(renderer, score_surf);
    SDL_Rect score_rect = {(DIS_WIDTH - score_surf->w) / 2, DIS_HEIGHT / 3 + 80, score_surf->w, score_surf->h};
    SDL_RenderCopy(renderer, score_tex, NULL, &score_rect);
    SDL_FreeSurface(score_surf);
    SDL_DestroyTexture(score_tex);
}
