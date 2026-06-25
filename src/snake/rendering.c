#include <math.h>
#include <SDL2/SDL.h>
#include "snake.h"
#include "core/graphics/graphics.h"

static int clamp_color(int value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return value;
}

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
        case SKIN_HOLOGRAPHIC:
            return is_head ? (Color){215, 180, 255} : (Color){170, 235, 255};
        case SKIN_MONOCHROME:
            return is_head ? (Color){220, 220, 220} : (Color){110, 110, 110};
        case SKIN_CHROMATIC:
            return is_head ? (Color){240, 150, 255} : (Color){90, 220, 240};
        case SKIN_COSMIC:
            return is_head ? (Color){190, 140, 255} : (Color){40, 30, 80};
        case SKIN_CHROME:
            return is_head ? (Color){220, 225, 235} : (Color){150, 170, 190};
        case SKIN_BUBBLE:
            return is_head ? (Color){190, 245, 255} : (Color){140, 220, 235};
        case SKIN_SHADOW:
            return is_head ? (Color){140, 110, 190} : (Color){40, 40, 65};
        case SKIN_SOLAR:
            return is_head ? (Color){255, 205, 60} : (Color){220, 120, 35};
        case SKIN_AQUA:
            return is_head ? (Color){0, 200, 220} : (Color){0, 140, 170};
        case SKIN_MIDNIGHT:
            return is_head ? (Color){90, 110, 170} : (Color){25, 30, 55};
        default:
            return is_head ? (Color){GREEN_R, GREEN_G, GREEN_B} : (Color){60, 160, 100};
    }
}

static Color get_skin_effect_color(SnakeSkin skin, int index, int is_head) {
    Color base_color = get_skin_color(skin, is_head);
    double time = SDL_GetTicks() / 200.0;
    if (skin == SKIN_RAINBOW) {
        double phase = index * 0.3 + (is_head ? 0.5 : 0.0) + time;
        int r = clamp_color((int)(128 + 127 * sin(phase)));
        int g = clamp_color((int)(128 + 127 * sin(phase + 2.094)));
        int b = clamp_color((int)(128 + 127 * sin(phase + 4.188)));
        double shimmer = 0.5 + 0.5 * sin(time * 2 + phase);
        return (Color){clamp_color((int)(r * (0.8 + 0.2 * shimmer))), clamp_color((int)(g * (0.8 + 0.2 * shimmer))), clamp_color((int)(b * (0.8 + 0.2 * shimmer)))};
    }
    if (skin == SKIN_NEON) {
        int glow = clamp_color((int)(100 * (0.5 + 0.5 * sin(time * 2 * M_PI))));
        return (Color){clamp_color(base_color.r + glow), base_color.g, clamp_color(base_color.b + glow)};
    }
    if (skin == SKIN_GOLD) {
        int shine = clamp_color((int)(50 * (0.5 + 0.5 * sin(time * 2 * M_PI))));
        return (Color){clamp_color(base_color.r + shine), clamp_color(base_color.g + shine), base_color.b};
    }
    if (skin == SKIN_HOLOGRAPHIC) {
        double phase = time + index * 0.35;
        int r = clamp_color((int)(200 + 50 * sin(phase + 0.5)));
        int g = clamp_color((int)(185 + 50 * sin(phase + 1.1)));
        int b = clamp_color((int)(220 + 40 * sin(phase + 2.3)));
        return (Color){r, g, b};
    }
    if (skin == SKIN_MONOCHROME) {
        int shade = 120 + (index % 3) * 12;
        return (Color){shade, shade, shade};
    }
    if (skin == SKIN_CHROMATIC) {
        double phase = time + index * 0.4;
        int r = clamp_color((int)(120 + 120 * sin(phase + 0.0)));
        int g = clamp_color((int)(120 + 120 * sin(phase + 2.0)));
        int b = clamp_color((int)(120 + 120 * sin(phase + 4.0)));
        return (Color){r, g, b};
    }
    if (skin == SKIN_COSMIC) {
        int glow = clamp_color((int)(25 * (0.5 + 0.5 * sin(time + index * 0.5))));
        return (Color){clamp_color(base_color.r + glow), base_color.g, clamp_color(base_color.b + glow)};
    }
    if (skin == SKIN_CHROME) {
        int shine = clamp_color((int)(15 + 20 * fabs(sin(time + index * 0.3))));
        return (Color){clamp_color(base_color.r + shine), clamp_color(base_color.g + shine), clamp_color(base_color.b + shine)};
    }
    if (skin == SKIN_BUBBLE) {
        int glow = clamp_color((int)(20 + 20 * sin(time + index * 0.4)));
        return (Color){clamp_color(base_color.r + glow), clamp_color(base_color.g + glow), clamp_color(base_color.b + glow)};
    }
    if (skin == SKIN_SHADOW) {
        int depth = 30 + (index % 2) * 10;
        return (Color){clamp_color(base_color.r + depth), clamp_color(base_color.g + depth / 2), clamp_color(base_color.b + depth)};
    }
    if (skin == SKIN_SOLAR) {
        int heat = clamp_color((int)(20 + 25 * sin(time + index * 0.25)));
        return (Color){base_color.r, clamp_color(base_color.g + heat), base_color.b};
    }
    if (skin == SKIN_AQUA) {
        int ripple = clamp_color((int)(20 + 20 * sin(time + index * 0.35)));
        return (Color){base_color.r, clamp_color(base_color.g + ripple), clamp_color(base_color.b + ripple)};
    }
    if (skin == SKIN_MIDNIGHT) {
        int pulse = clamp_color((int)(15 + 15 * sin(time * 0.5 + index * 0.2)));
        return (Color){base_color.r, base_color.g, clamp_color(base_color.b + pulse)};
    }
    return base_color;
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

void draw_snake(SDL_Renderer* renderer, const Snake* snake, int glow_active, int frozen) {
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
        } else if (snake->skin == SKIN_HOLOGRAPHIC) {
            double time = SDL_GetTicks() / 400.0;
            double phase = time + i * 0.35;
            int r = (int)(200 + 50 * sin(phase + 0.5));
            int g = (int)(185 + 50 * sin(phase + 1.1));
            int b = (int)(220 + 40 * sin(phase + 2.3));
            base_color = (Color){r, g, b};
        } else if (snake->skin == SKIN_MONOCHROME) {
            int shade = 120 + (i % 3) * 12;
            base_color = (Color){shade, shade, shade};
        } else if (snake->skin == SKIN_CHROMATIC) {
            double time = SDL_GetTicks() / 220.0;
            double phase = time + i * 0.4;
            int r = (int)(120 + 120 * sin(phase + 0.0));
            int g = (int)(120 + 120 * sin(phase + 2.0));
            int b = (int)(120 + 120 * sin(phase + 4.0));
            base_color = (Color){r, g, b};
        } else if (snake->skin == SKIN_COSMIC) {
            double time = SDL_GetTicks() / 500.0;
            int glow = (int)(25 * (0.5 + 0.5 * sin(time + i * 0.5)));
            base_color = (Color){
                base_color.r + glow > 255 ? 255 : base_color.r + glow,
                base_color.g,
                base_color.b + glow > 255 ? 255 : base_color.b + glow
            };
        } else if (snake->skin == SKIN_CHROME) {
            double time = SDL_GetTicks() / 320.0;
            int shine = (int)(15 + 20 * fabs(sin(time + i * 0.3)));
            base_color = (Color){
                base_color.r + shine > 255 ? 255 : base_color.r + shine,
                base_color.g + shine > 255 ? 255 : base_color.g + shine,
                base_color.b + shine > 255 ? 255 : base_color.b + shine
            };
        } else if (snake->skin == SKIN_BUBBLE) {
            double time = SDL_GetTicks() / 300.0;
            int glow = (int)(20 + 20 * sin(time + i * 0.4));
            base_color = (Color){
                base_color.r + glow > 255 ? 255 : base_color.r + glow,
                base_color.g + glow > 255 ? 255 : base_color.g + glow,
                base_color.b + glow > 255 ? 255 : base_color.b + glow
            };
        } else if (snake->skin == SKIN_SHADOW) {
            int depth = 30 + (i % 2) * 10;
            base_color = (Color){
                base_color.r + depth > 255 ? 255 : base_color.r + depth,
                base_color.g + depth / 2 > 255 ? 255 : base_color.g + depth / 2,
                base_color.b + depth > 255 ? 255 : base_color.b + depth
            };
        } else if (snake->skin == SKIN_SOLAR) {
            double time = SDL_GetTicks() / 360.0;
            int heat = (int)(20 + 25 * sin(time + i * 0.25));
            base_color = (Color){
                base_color.r,
                base_color.g + heat > 255 ? 255 : base_color.g + heat,
                base_color.b
            };
        } else if (snake->skin == SKIN_AQUA) {
            double time = SDL_GetTicks() / 280.0;
            int ripple = (int)(20 + 20 * sin(time + i * 0.35));
            base_color = (Color){
                base_color.r,
                base_color.g + ripple > 255 ? 255 : base_color.g + ripple,
                base_color.b + ripple > 255 ? 255 : base_color.b + ripple
            };
        } else if (snake->skin == SKIN_MIDNIGHT) {
            double time = SDL_GetTicks() / 720.0;
            int pulse = (int)(15 + 15 * sin(time + i * 0.2));
            base_color = (Color){
                base_color.r,
                base_color.g,
                base_color.b + pulse > 255 ? 255 : base_color.b + pulse
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
        if (frozen) {
            int r = (color.r + 180) / 2;
            int g = (color.g + 220) / 2;
            int b = (color.b + 255) / 2;
            color = (Color){r, g, b};
        }
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
        if (frozen) {
            int r = (color.r + 180) / 2;
            int g = (color.g + 220) / 2;
            int b = (color.b + 255) / 2;
            color = (Color){r, g, b};
        }
        draw_cube(renderer, head.x, head.y, SNAKE_BLOCK, color, 1);

        SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        SDL_Rect head_rect = {head.x, head.y, SNAKE_BLOCK, SNAKE_BLOCK};
        SDL_RenderDrawRect(renderer, &head_rect);
    }
}
