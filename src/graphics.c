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
        case SKIN_PURPLE:
            return is_head ? (Color){180, 100, 255} : (Color){140, 70, 200};
        case SKIN_GOLD:
            return is_head ? (Color){255, 215, 0} : (Color){200, 160, 0};
        case SKIN_CYAN:
            return is_head ? (Color){0, 255, 255} : (Color){0, 200, 200};
        case SKIN_NEON:
            return is_head ? (Color){255, 0, 255} : (Color){200, 0, 200};
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
        case POWERUP_PATHFIND: return (Color){100, 255, 150};
        case POWERUP_FRENZY: return (Color){255, 150, 50};
        default: return (Color){WHITE_R, WHITE_G, WHITE_B};
    }
}

static SDL_Rect draw_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y) {
    SDL_Rect rect = {x, y, 0, 0};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) {
        return rect;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return rect;
    }

    rect.w = surface->w;
    rect.h = surface->h;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    return rect;
}

static SDL_Rect draw_text_centered(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, SDL_Rect bounds) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Rect rect = {bounds.x, bounds.y, 0, 0};
    if (!surface) {
        return rect;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return rect;
    }

    rect.w = surface->w;
    rect.h = surface->h;
    rect.x = bounds.x + (bounds.w - rect.w) / 2;
    rect.y = bounds.y + (bounds.h - rect.h) / 2;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    return rect;
}

static void fill_rect(SDL_Renderer* renderer, SDL_Rect rect, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

static void stroke_rect(SDL_Renderer* renderer, SDL_Rect rect, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

static void draw_panel(SDL_Renderer* renderer, SDL_Rect rect, Color fill, Color border) {
    SDL_Rect shadow = {rect.x + 6, rect.y + 7, rect.w, rect.h};
    fill_rect(renderer, shadow, (Color){6, 9, 14});
    fill_rect(renderer, rect, fill);
    stroke_rect(renderer, rect, border);
}

static Color apply_powerup_glow(Color base, PowerupType active_powerup) {
    if (active_powerup != POWERUP_INVINCIBLE && active_powerup != POWERUP_PATHFIND) {
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
    SDL_Rect rect = {x + 1, y + 1, size - 2, size - 2};

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

    SDL_SetRenderDrawColor(renderer, highlight.r, highlight.g, highlight.b, 255);
    SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w - 1, rect.y);
    SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h - 1);

    SDL_SetRenderDrawColor(renderer, shadow.r, shadow.g, shadow.b, 255);
    SDL_RenderDrawLine(renderer, rect.x + 1, rect.y + rect.h - 1, rect.x + rect.w - 1, rect.y + rect.h - 1);
    SDL_RenderDrawLine(renderer, rect.x + rect.w - 1, rect.y + 1, rect.x + rect.w - 1, rect.y + rect.h - 1);

    if (is_head) {
        Color inner_highlight = {
            color.r + 100 > 255 ? 255 : color.r + 100,
            color.g + 100 > 255 ? 255 : color.g + 100,
            color.b + 60 > 255 ? 255 : color.b + 60
        };
        SDL_Rect inner_rect = {rect.x + 3, rect.y + 3, size / 3, size / 3};
        SDL_SetRenderDrawColor(renderer, inner_highlight.r, inner_highlight.g, inner_highlight.b, 255);
        SDL_RenderFillRect(renderer, &inner_rect);
    }
}

void draw_grid(SDL_Renderer* renderer) {
    SDL_Rect area = {0, GAME_AREA_TOP, DIS_WIDTH, GAME_AREA_HEIGHT};
    fill_rect(renderer, area, (Color){12, 16, 23});

    for (int y = GAME_AREA_TOP; y < GAME_AREA_BOTTOM; y += SNAKE_BLOCK) {
        int band = ((y - GAME_AREA_TOP) / SNAKE_BLOCK) % 2;
        SDL_Rect row = {0, y, DIS_WIDTH, SNAKE_BLOCK};
        fill_rect(renderer, row, band ? (Color){14, 19, 28} : (Color){16, 22, 32});
    }

    SDL_SetRenderDrawColor(renderer, 26, 36, 50, 255);
    for (int x = 0; x <= DIS_WIDTH; x += SNAKE_BLOCK) {
        SDL_RenderDrawLine(renderer, x, GAME_AREA_TOP, x, GAME_AREA_BOTTOM - 1);
    }

    SDL_SetRenderDrawColor(renderer, 30, 42, 58, 255);
    for (int y = GAME_AREA_TOP; y <= GAME_AREA_BOTTOM; y += SNAKE_BLOCK) {
        SDL_RenderDrawLine(renderer, 0, y, DIS_WIDTH - 1, y);
    }

    SDL_SetRenderDrawColor(renderer, 42, 62, 84, 255);
    for (int x = 0; x <= DIS_WIDTH; x += SNAKE_BLOCK * 5) {
        SDL_RenderDrawLine(renderer, x, GAME_AREA_TOP, x, GAME_AREA_BOTTOM - 1);
    }
    for (int y = GAME_AREA_TOP; y <= GAME_AREA_BOTTOM; y += SNAKE_BLOCK * 5) {
        SDL_RenderDrawLine(renderer, 0, y, DIS_WIDTH - 1, y);
    }

    SDL_SetRenderDrawColor(renderer, 86, 122, 150, 255);
    SDL_RenderDrawLine(renderer, 0, GAME_AREA_TOP, DIS_WIDTH - 1, GAME_AREA_TOP);
    SDL_RenderDrawLine(renderer, 0, GAME_AREA_BOTTOM - 1, DIS_WIDTH - 1, GAME_AREA_BOTTOM - 1);
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
        } else if (snake->skin == SKIN_NEON) {
            double pulse = (SDL_GetTicks() % 500) / 500.0;
            int glow = (int)(80 * (0.5 + 0.5 * sin(pulse * 2 * M_PI)));
            base_color = (Color){
                base_color.r + glow > 255 ? 255 : base_color.r + glow,
                base_color.g,
                base_color.b + glow > 255 ? 255 : base_color.b + glow
            };
        } else if (snake->skin == SKIN_GOLD) {
            double shimmer = (SDL_GetTicks() % 300) / 300.0;
            int shine = (int)(40 * (0.5 + 0.5 * sin(shimmer * 2 * M_PI)));
            base_color = (Color){
                base_color.r + shine > 255 ? 255 : base_color.r + shine,
                base_color.g + shine > 255 ? 255 : base_color.g + shine,
                base_color.b
            };
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
        } else if (snake->skin == SKIN_NEON) {
            double pulse = (SDL_GetTicks() % 500) / 500.0;
            int glow = (int)(100 * (0.5 + 0.5 * sin(pulse * 2 * M_PI)));
            base_color = (Color){
                base_color.r + glow > 255 ? 255 : base_color.r + glow,
                base_color.g,
                base_color.b + glow > 255 ? 255 : base_color.b + glow
            };
        } else if (snake->skin == SKIN_GOLD) {
            double shimmer = (SDL_GetTicks() % 300) / 300.0;
            int shine = (int)(50 * (0.5 + 0.5 * sin(shimmer * 2 * M_PI)));
            base_color = (Color){
                base_color.r + shine > 255 ? 255 : base_color.r + shine,
                base_color.g + shine > 255 ? 255 : base_color.g + shine,
                base_color.b
            };
        }

        Color color = apply_powerup_glow(base_color, active_powerup);
        draw_cube(renderer, head.x, head.y, SNAKE_BLOCK, color, 1);

        SDL_SetRenderDrawColor(renderer, 230, 250, 245, 255);
        SDL_Rect head_rect = {head.x + 1, head.y + 1, SNAKE_BLOCK - 2, SNAKE_BLOCK - 2};
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
              int current_speed, PowerupType active_powerup, double powerup_end_time,
              GameMode mode, double time_left) {
    SDL_Rect header_rect = {0, 0, DIS_WIDTH, HEADER_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 12, 18, 28, 255);
    SDL_RenderFillRect(renderer, &header_rect);

    SDL_Rect border_rect = {8, 8, DIS_WIDTH - 16, HEADER_HEIGHT - 16};
    SDL_SetRenderDrawColor(renderer, 24, 32, 46, 255);
    SDL_RenderFillRect(renderer, &border_rect);
    SDL_SetRenderDrawColor(renderer, 60, 90, 130, 255);
    SDL_RenderDrawRect(renderer, &border_rect);

    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    SDL_Color accent = {120, 195, 255, 255};
    SDL_Color muted = {170, 180, 190, 255};

    char score_label[16];
    snprintf(score_label, sizeof(score_label), "SCORE");
    SDL_Surface* score_label_surf = TTF_RenderText_Blended(button_font, score_label, muted);
    SDL_Texture* score_label_tex = SDL_CreateTextureFromSurface(renderer, score_label_surf);
    SDL_Rect score_label_rect = {28, 20, score_label_surf->w, score_label_surf->h};
    SDL_RenderCopy(renderer, score_label_tex, NULL, &score_label_rect);
    SDL_FreeSurface(score_label_surf);
    SDL_DestroyTexture(score_label_tex);

    char score_value[32];
    snprintf(score_value, sizeof(score_value), "%d", score);
    SDL_Surface* score_value_surf = TTF_RenderText_Blended(score_font, score_value, accent);
    SDL_Texture* score_value_tex = SDL_CreateTextureFromSurface(renderer, score_value_surf);
    SDL_Rect score_value_rect = {28, 32 + score_label_rect.h - 4, score_value_surf->w, score_value_surf->h};
    SDL_RenderCopy(renderer, score_value_tex, NULL, &score_value_rect);
    SDL_FreeSurface(score_value_surf);
    SDL_DestroyTexture(score_value_tex);

    char high_label[16];
    snprintf(high_label, sizeof(high_label), "HIGH");
    SDL_Surface* high_label_surf = TTF_RenderText_Blended(button_font, high_label, muted);
    SDL_Texture* high_label_tex = SDL_CreateTextureFromSurface(renderer, high_label_surf);
    SDL_Rect high_label_rect = {DIS_WIDTH - high_label_surf->w - 28, 20, high_label_surf->w, high_label_surf->h};
    SDL_RenderCopy(renderer, high_label_tex, NULL, &high_label_rect);
    SDL_FreeSurface(high_label_surf);
    SDL_DestroyTexture(high_label_tex);

    char high_value[32];
    snprintf(high_value, sizeof(high_value), "%d", high_score);
    SDL_Surface* high_value_surf = TTF_RenderText_Blended(score_font, high_value, white);
    SDL_Texture* high_value_tex = SDL_CreateTextureFromSurface(renderer, high_value_surf);
    SDL_Rect high_value_rect = {DIS_WIDTH - high_value_surf->w - 28, 32 + high_label_rect.h - 4, high_value_surf->w, high_value_surf->h};
    SDL_RenderCopy(renderer, high_value_tex, NULL, &high_value_rect);
    SDL_FreeSurface(high_value_surf);
    SDL_DestroyTexture(high_value_tex);

    int chip_width = 130;
    int chip_height = 24;
    int chip_y = HEADER_HEIGHT - chip_height - 12;
    int chip_x = 28;

    char mode_text[24];
    const char* mode_name = mode == MODE_CHALLENGE ? "CHALLENGE" : mode == MODE_TIME_ATTACK ? "TIME ATTACK" : "CLASSIC";
    snprintf(mode_text, sizeof(mode_text), "%s", mode_name);
    SDL_Rect mode_chip = {chip_x, chip_y, chip_width, chip_height};
    SDL_SetRenderDrawColor(renderer, 22, 32, 44, 255);
    SDL_RenderFillRect(renderer, &mode_chip);
    SDL_SetRenderDrawColor(renderer, 70, 110, 160, 255);
    SDL_RenderDrawRect(renderer, &mode_chip);
    SDL_Surface* mode_surf = TTF_RenderText_Blended(button_font, mode_text, white);
    SDL_Texture* mode_tex = SDL_CreateTextureFromSurface(renderer, mode_surf);
    SDL_Rect mode_rect = {mode_chip.x + 10, mode_chip.y + 4, mode_surf->w, mode_surf->h};
    SDL_RenderCopy(renderer, mode_tex, NULL, &mode_rect);
    SDL_FreeSurface(mode_surf);
    SDL_DestroyTexture(mode_tex);

    chip_x += chip_width + 12;
    char speed_text[24];
    snprintf(speed_text, sizeof(speed_text), "SPEED %d", current_speed);
    SDL_Rect speed_chip = {chip_x, chip_y, chip_width, chip_height};
    SDL_SetRenderDrawColor(renderer, 22, 32, 44, 255);
    SDL_RenderFillRect(renderer, &speed_chip);
    SDL_SetRenderDrawColor(renderer, 70, 110, 160, 255);
    SDL_RenderDrawRect(renderer, &speed_chip);
    SDL_Surface* speed_surf = TTF_RenderText_Blended(button_font, speed_text, white);
    SDL_Texture* speed_tex = SDL_CreateTextureFromSurface(renderer, speed_surf);
    SDL_Rect speed_rect = {speed_chip.x + 10, speed_chip.y + 4, speed_surf->w, speed_surf->h};
    SDL_RenderCopy(renderer, speed_tex, NULL, &speed_rect);
    SDL_FreeSurface(speed_surf);
    SDL_DestroyTexture(speed_tex);

    chip_x += chip_width + 12;
    char apples_text[24];
    snprintf(apples_text, sizeof(apples_text), "APPLES %d", apples_collected);
    SDL_Rect apples_chip = {chip_x, chip_y, chip_width, chip_height};
    SDL_SetRenderDrawColor(renderer, 22, 32, 44, 255);
    SDL_RenderFillRect(renderer, &apples_chip);
    SDL_SetRenderDrawColor(renderer, 70, 110, 160, 255);
    SDL_RenderDrawRect(renderer, &apples_chip);
    SDL_Surface* apples_surf = TTF_RenderText_Blended(button_font, apples_text, white);
    SDL_Texture* apples_tex = SDL_CreateTextureFromSurface(renderer, apples_surf);
    SDL_Rect apples_rect = {apples_chip.x + 10, apples_chip.y + 4, apples_surf->w, apples_surf->h};
    SDL_RenderCopy(renderer, apples_tex, NULL, &apples_rect);
    SDL_FreeSurface(apples_surf);
    SDL_DestroyTexture(apples_tex);

    chip_x += chip_width + 12;
    char len_text[24];
    snprintf(len_text, sizeof(len_text), "LEN %d", snake_length);
    SDL_Rect len_chip = {chip_x, chip_y, chip_width, chip_height};
    SDL_SetRenderDrawColor(renderer, 22, 32, 44, 255);
    SDL_RenderFillRect(renderer, &len_chip);
    SDL_SetRenderDrawColor(renderer, 70, 110, 160, 255);
    SDL_RenderDrawRect(renderer, &len_chip);
    SDL_Surface* len_surf = TTF_RenderText_Blended(button_font, len_text, white);
    SDL_Texture* len_tex = SDL_CreateTextureFromSurface(renderer, len_surf);
    SDL_Rect len_rect = {len_chip.x + 10, len_chip.y + 4, len_surf->w, len_surf->h};
    SDL_RenderCopy(renderer, len_tex, NULL, &len_rect);
    SDL_FreeSurface(len_surf);
    SDL_DestroyTexture(len_tex);

    if (mode == MODE_TIME_ATTACK) {
        int bar_width = 220;
        int bar_height = 12;
        SDL_Rect time_bg = {DIS_WIDTH - bar_width - 28, HEADER_HEIGHT - bar_height - 14, bar_width, bar_height};
        SDL_SetRenderDrawColor(renderer, 30, 40, 56, 255);
        SDL_RenderFillRect(renderer, &time_bg);
        SDL_Rect time_border = {time_bg.x, time_bg.y, time_bg.w, time_bg.h};
        SDL_SetRenderDrawColor(renderer, 70, 120, 190, 255);
        SDL_RenderDrawRect(renderer, &time_border);

        double progress = time_left / TIME_ATTACK_DURATION;
        if (progress < 0.0) progress = 0.0;
        if (progress > 1.0) progress = 1.0;
        SDL_Rect time_fg = {time_bg.x, time_bg.y, (int)(bar_width * progress), bar_height};
        SDL_SetRenderDrawColor(renderer, 120, 195, 255, 255);
        SDL_RenderFillRect(renderer, &time_fg);

        char timer_text[32];
        snprintf(timer_text, sizeof(timer_text), "TIME %.0fs", time_left);
        SDL_Surface* timer_surf = TTF_RenderText_Blended(button_font, timer_text, white);
        SDL_Texture* timer_tex = SDL_CreateTextureFromSurface(renderer, timer_surf);
        SDL_Rect timer_rect = {time_bg.x + (bar_width - timer_surf->w) / 2, time_bg.y - timer_surf->h - 4, timer_surf->w, timer_surf->h};
        SDL_RenderCopy(renderer, timer_tex, NULL, &timer_rect);
        SDL_FreeSurface(timer_surf);
        SDL_DestroyTexture(timer_tex);
    }

    if (active_powerup != POWERUP_NONE) {
        const char* powerup_names[] = {"SPEED BOOST", "SLOW MOTION", "2X POINTS", "INVINCIBLE", "PATHFIND", "FRENZY"};
        const char* name = powerup_names[active_powerup];
        double powerup_time_left = powerup_end_time - SDL_GetTicks() / 1000.0;
        if (powerup_time_left < 0.0) powerup_time_left = 0.0;
        char power_text[64];
        snprintf(power_text, sizeof(power_text), "%s %.1fs", name, powerup_time_left);
        SDL_Rect power_chip = {DIS_WIDTH - 268, GAME_AREA_TOP + 12, 240, 28};
        SDL_SetRenderDrawColor(renderer, 24, 34, 48, 255);
        SDL_RenderFillRect(renderer, &power_chip);
        SDL_SetRenderDrawColor(renderer, 80, 150, 220, 255);
        SDL_RenderDrawRect(renderer, &power_chip);
        SDL_Surface* power_surf = TTF_RenderText_Blended(button_font, power_text, accent);
        SDL_Texture* power_tex = SDL_CreateTextureFromSurface(renderer, power_surf);
        SDL_Rect power_rect = {power_chip.x + 12, power_chip.y + 4, power_surf->w, power_surf->h};
        SDL_RenderCopy(renderer, power_tex, NULL, &power_rect);
        SDL_FreeSurface(power_surf);
        SDL_DestroyTexture(power_tex);
    }
}

void draw_main_menu(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* button_font) {
    SDL_SetRenderDrawColor(renderer, 9, 12, 18, 255);
    SDL_RenderClear(renderer);

    SDL_Color accent = {124, 218, 184, 255};
    SDL_Color amber = {255, 196, 92, 255};
    SDL_Color muted = {158, 170, 184, 255};
    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};

    SDL_SetRenderDrawColor(renderer, 18, 27, 36, 255);
    for (int x = 0; x < DIS_WIDTH; x += 40) {
        SDL_RenderDrawLine(renderer, x, 0, x, DIS_HEIGHT);
    }
    for (int y = 0; y < DIS_HEIGHT; y += 40) {
        SDL_RenderDrawLine(renderer, 0, y, DIS_WIDTH, y);
    }

    SDL_Rect shell = {88, 82, DIS_WIDTH - 176, DIS_HEIGHT - 164};
    draw_panel(renderer, shell, (Color){16, 22, 31}, (Color){58, 83, 98});

    SDL_Rect hero = {122, 122, 430, 520};
    fill_rect(renderer, hero, (Color){13, 19, 27});
    stroke_rect(renderer, hero, (Color){45, 76, 72});

    SDL_Rect rail = {hero.x, hero.y, 5, hero.h};
    fill_rect(renderer, rail, (Color){124, 218, 184});

    SDL_Rect title_area = {hero.x + 36, hero.y + 34, hero.w - 72, 80};
    draw_text(renderer, large_font, "SNEK", accent, title_area.x, title_area.y);
    draw_text(renderer, button_font, "Fast, clean, bite-sized arcade survival.", muted, title_area.x, title_area.y + 52);

    int preview_x = hero.x + 58;
    int preview_y = hero.y + 170;
    SDL_Rect preview = {preview_x - 24, preview_y - 34, 300, 210};
    fill_rect(renderer, preview, (Color){17, 25, 34});
    stroke_rect(renderer, preview, (Color){49, 73, 87});
    SDL_SetRenderDrawColor(renderer, 28, 40, 52, 255);
    for (int x = preview.x; x <= preview.x + preview.w; x += 20) {
        SDL_RenderDrawLine(renderer, x, preview.y, x, preview.y + preview.h);
    }
    for (int y = preview.y; y <= preview.y + preview.h; y += 20) {
        SDL_RenderDrawLine(renderer, preview.x, y, preview.x + preview.w, y);
    }

    Color snake_head = {124, 218, 184};
    Color snake_body = {70, 156, 124};
    for (int i = 0; i < 8; i++) {
        int px = preview_x + i * SNAKE_BLOCK;
        int py = preview_y + (i > 4 ? SNAKE_BLOCK : 0);
        draw_cube(renderer, px, py, SNAKE_BLOCK, i == 7 ? snake_head : snake_body, i == 7);
    }
    SDL_Rect food = {preview_x + 220, preview_y + 80, SNAKE_BLOCK - 2, SNAKE_BLOCK - 2};
    fill_rect(renderer, food, (Color){255, 92, 98});
    stroke_rect(renderer, food, (Color){255, 184, 138});

    SDL_Rect badge1 = {hero.x + 36, hero.y + 430, 105, 46};
    SDL_Rect badge2 = {hero.x + 156, hero.y + 430, 105, 46};
    SDL_Rect badge3 = {hero.x + 276, hero.y + 430, 105, 46};
    fill_rect(renderer, badge1, (Color){21, 30, 40});
    fill_rect(renderer, badge2, (Color){21, 30, 40});
    fill_rect(renderer, badge3, (Color){21, 30, 40});
    stroke_rect(renderer, badge1, (Color){55, 77, 90});
    stroke_rect(renderer, badge2, (Color){55, 77, 90});
    stroke_rect(renderer, badge3, (Color){55, 77, 90});
    draw_text_centered(renderer, button_font, "8 SKINS", white, badge1);
    draw_text_centered(renderer, button_font, "3 MODES", white, badge2);
    draw_text_centered(renderer, button_font, "POWERUPS", white, badge3);

    SDL_Rect action_panel = {594, 150, 282, 470};
    fill_rect(renderer, action_panel, (Color){20, 29, 39});
    stroke_rect(renderer, action_panel, (Color){69, 92, 105});
    draw_text(renderer, button_font, "MAIN MENU", amber, action_panel.x + 32, action_panel.y + 32);
    draw_text(renderer, button_font, "WASD or arrows in game", muted, action_panel.x + 32, action_panel.y + 62);
    draw_text(renderer, button_font, "ESC pauses or backs out", muted, action_panel.x + 32, action_panel.y + 86);
}

void draw_settings_menu(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* score_font,
                        TTF_Font* button_font, GameState* state, int current_option) {
    (void)current_option;
    SDL_SetRenderDrawColor(renderer, 8, 11, 17, 255);
    SDL_RenderClear(renderer);

    SDL_Color accent = {124, 218, 184, 255};
    SDL_Color amber = {255, 196, 92, 255};
    SDL_Color muted = {158, 170, 184, 255};
    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};

    SDL_SetRenderDrawColor(renderer, 17, 25, 33, 255);
    for (int x = 0; x < DIS_WIDTH; x += 40) {
        SDL_RenderDrawLine(renderer, x, 0, x, DIS_HEIGHT);
    }
    for (int y = 0; y < DIS_HEIGHT; y += 40) {
        SDL_RenderDrawLine(renderer, 0, y, DIS_WIDTH, y);
    }

    SDL_Rect frame = {70, 64, DIS_WIDTH - 140, DIS_HEIGHT - 128};
    draw_panel(renderer, frame, (Color){15, 21, 30}, (Color){57, 80, 95});

    SDL_Rect left_panel = {104, 104, 286, 592};
    SDL_Rect right_panel = {430, 104, 466, 592};
    fill_rect(renderer, left_panel, (Color){19, 28, 38});
    fill_rect(renderer, right_panel, (Color){18, 26, 36});
    stroke_rect(renderer, left_panel, (Color){52, 76, 89});
    stroke_rect(renderer, right_panel, (Color){52, 76, 89});

    draw_text(renderer, large_font, "SETTINGS", accent, right_panel.x + 28, right_panel.y + 26);
    draw_text(renderer, button_font, "Click a row to cycle its value.", muted, right_panel.x + 30, right_panel.y + 78);
    draw_text(renderer, button_font, "Changes save immediately.", muted, right_panel.x + 30, right_panel.y + 102);

    const char* skin_names[] = {"Default", "Blue", "Red", "Rainbow", "Purple", "Gold", "Cyan", "Neon"};
    const char* mode_names[] = {"Classic", "Challenge", "Time Attack"};
    const char* difficulty_names[] = {"Easy", "Normal", "Hard"};

    draw_text(renderer, button_font, "SKIN PREVIEW", amber, left_panel.x + 24, left_panel.y + 28);
    draw_text(renderer, score_font, skin_names[state->settings.current_skin], white, left_panel.x + 24, left_panel.y + 58);

    SDL_Rect preview_box = {left_panel.x + 24, left_panel.y + 112, left_panel.w - 48, 190};
    fill_rect(renderer, preview_box, (Color){12, 18, 26});
    stroke_rect(renderer, preview_box, (Color){49, 73, 87});
    SDL_SetRenderDrawColor(renderer, 27, 38, 50, 255);
    for (int x = preview_box.x; x <= preview_box.x + preview_box.w; x += 20) {
        SDL_RenderDrawLine(renderer, x, preview_box.y, x, preview_box.y + preview_box.h);
    }
    for (int y = preview_box.y; y <= preview_box.y + preview_box.h; y += 20) {
        SDL_RenderDrawLine(renderer, preview_box.x, y, preview_box.x + preview_box.w, y);
    }

    for (int i = 0; i < 6; i++) {
        int px = preview_box.x + 48 + i * SNAKE_BLOCK;
        int py = preview_box.y + 82;
        draw_cube(renderer, px, py, SNAKE_BLOCK, get_skin_color(state->settings.current_skin, i == 5), i == 5);
    }

    SDL_Rect details = {left_panel.x + 24, left_panel.y + 330, left_panel.w - 48, 208};
    fill_rect(renderer, details, (Color){21, 30, 40});
    stroke_rect(renderer, details, (Color){52, 76, 89});
    draw_text(renderer, button_font, "CURRENT LOADOUT", amber, details.x + 18, details.y + 18);
    draw_text(renderer, button_font, mode_names[state->settings.mode], white, details.x + 18, details.y + 58);
    draw_text(renderer, button_font, difficulty_names[state->settings.difficulty], white, details.x + 18, details.y + 86);
    draw_text(renderer, button_font, state->settings.sound_enabled ? "Sound on" : "Sound off", muted, details.x + 18, details.y + 122);
    draw_text(renderer, button_font, state->settings.music_enabled ? "Music on" : "Music off", muted, details.x + 18, details.y + 148);
}

void draw_game_over(SDL_Renderer* renderer, TTF_Font* large_font, TTF_Font* score_font,
                   TTF_Font* button_font, int score) {
    (void)button_font;
    SDL_SetRenderDrawColor(renderer, 10, 14, 22, 255);
    SDL_RenderClear(renderer);

    SDL_Rect card = {180, 170, DIS_WIDTH - 360, DIS_HEIGHT - 340};
    SDL_SetRenderDrawColor(renderer, 24, 32, 46, 255);
    SDL_RenderFillRect(renderer, &card);
    SDL_SetRenderDrawColor(renderer, 90, 140, 210, 255);
    SDL_RenderDrawRect(renderer, &card);

    SDL_Color accent = {150, 220, 255, 255};
    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    SDL_Color gray = {170, 180, 190, 255};

    SDL_Surface* title_surf = TTF_RenderText_Blended(large_font, "GAME OVER", accent);
    SDL_Texture* title_tex = SDL_CreateTextureFromSurface(renderer, title_surf);
    SDL_Rect title_rect = {(DIS_WIDTH - title_surf->w) / 2, 220, title_surf->w, title_surf->h};
    SDL_RenderCopy(renderer, title_tex, NULL, &title_rect);
    SDL_FreeSurface(title_surf);
    SDL_DestroyTexture(title_tex);

    char score_text[32];
    snprintf(score_text, sizeof(score_text), "Final Score: %d", score);
    SDL_Surface* score_surf = TTF_RenderText_Blended(score_font, score_text, white);
    SDL_Texture* score_tex = SDL_CreateTextureFromSurface(renderer, score_surf);
    SDL_Rect score_rect = {(DIS_WIDTH - score_surf->w) / 2, 295, score_surf->w, score_surf->h};
    SDL_RenderCopy(renderer, score_tex, NULL, &score_rect);
    SDL_FreeSurface(score_surf);
    SDL_DestroyTexture(score_tex);

    SDL_Surface* hint_surf = TTF_RenderText_Blended(button_font, "Hit PLAY AGAIN to try for a new high score.", gray);
    SDL_Texture* hint_tex = SDL_CreateTextureFromSurface(renderer, hint_surf);
    SDL_Rect hint_rect = {(DIS_WIDTH - hint_surf->w) / 2, 335, hint_surf->w, hint_surf->h};
    SDL_RenderCopy(renderer, hint_tex, NULL, &hint_rect);
    SDL_FreeSurface(hint_surf);
    SDL_DestroyTexture(hint_tex);
}
