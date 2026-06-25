#include <math.h>
#include <SDL2/SDL.h>
#include "snake.h"
#include "core/graphics/graphics.h"

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

static Color apply_powerup_glow(Color base, int glow_active) {
    if (!glow_active) return base;

    double pulse = (SDL_GetTicks() % 1000) / 1000.0;
    int highlight = 55 + (int)(100 * (0.5 + 0.5 * sin(pulse * 2 * M_PI)));

    return (Color){
        base.r + highlight > 255 ? 255 : base.r + highlight,
        base.g + highlight > 255 ? 255 : base.g + highlight,
        base.b + highlight > 255 ? 255 : base.b + highlight
    };
}

void draw_snake(SDL_Renderer* renderer, const Snake* snake, int glow_active) {
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

        Color color = apply_powerup_glow(base_color, glow_active);
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

        Color color = apply_powerup_glow(base_color, glow_active);
        draw_cube(renderer, head.x, head.y, SNAKE_BLOCK, color, 1);

        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        SDL_Rect head_rect = {head.x, head.y, SNAKE_BLOCK, SNAKE_BLOCK};
        SDL_RenderDrawRect(renderer, &head_rect);
    }
}
