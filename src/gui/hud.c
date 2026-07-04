#include "gui/hud.h"
#include "core/game/constants.h"
#include "gui/layout.h"
#include "core/graphics/graphics.h"
#include "items/apple/apple.h"
#include "items/bomb/bomb.h"
#include "items/banana/banana.h"
#include "powerups/registry.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

static void draw_text(SDL_Renderer *renderer,
                      TTF_Font *font,
                      const char *text,
                      SDL_Color color,
                      int x,
                      int y)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface)
    {
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

static void draw_panel(SDL_Renderer *renderer,
                       const GuiRect *rect,
                       SDL_Color fill,
                       SDL_Color border)
{
    SDL_Rect panel = {rect->x, rect->y, rect->w, rect->h};
    SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
    SDL_RenderDrawRect(renderer, &panel);
}

void gui_hud_render(GuiContext *gui, const GuiHudData *data)
{
    if (!gui || !data)
    {
        return;
    }

    SDL_Renderer *renderer = gui->renderer;
    GuiRect header = gui_make_rect(gui, 0.0f, 0.0f, 1.0f, 0.09f);
    SDL_SetRenderDrawColor(renderer, 12, 18, 28, 255);
    SDL_RenderFillRect(renderer, &(SDL_Rect){0, 0, gui->layout.width, header.h});

    GuiRect border = gui_make_rect(gui, 0.008f, 0.01f, 0.984f, 0.07f);
    draw_panel(renderer, &border, (SDL_Color){24, 32, 46, 255}, (SDL_Color){60, 90, 130, 255});

    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    SDL_Color accent = {120, 195, 255, 255};
    SDL_Color muted = {170, 180, 190, 255};

    char score_value[32];
    snprintf(score_value, sizeof(score_value), "%d", data->score);
    draw_text(renderer, gui->score_font, "SCORE", muted, 28, 20);
    draw_text(renderer, gui->score_font, score_value, accent, 28, 36);

    char high_value[32];
    snprintf(high_value, sizeof(high_value), "%d", data->high_score);
    draw_text(renderer, gui->score_font, "HIGH", muted, gui->layout.width - 120, 20);
    draw_text(renderer, gui->score_font, high_value, white, gui->layout.width - 120, 36);

    int chip_y = 36;
    int chip_x = 28 + 120;
    int chip_width = gui_layout_scale(&gui->layout, 108);
    int chip_height = 24;
    int chip_spacing = 8;

    const char *mode_name = data->mode == MODE_CHALLENGE ? "CHALLENGE" : data->mode == MODE_TIME_ATTACK ? "TIME ATTACK" : "CLASSIC";
    char mode_text[24];
    snprintf(mode_text, sizeof(mode_text), "%s", mode_name);
    SDL_Rect mode_chip = {chip_x, chip_y, chip_width, chip_height};
    SDL_SetRenderDrawColor(renderer, 22, 32, 44, 255);
    SDL_RenderFillRect(renderer, &mode_chip);
    SDL_SetRenderDrawColor(renderer, 70, 110, 160, 255);
    SDL_RenderDrawRect(renderer, &mode_chip);
    draw_text(renderer, gui->button_font, mode_text, white, chip_x + 8, chip_y + 4);

    chip_x += chip_width + chip_spacing;
    char speed_text[24];
    snprintf(speed_text, sizeof(speed_text), "SPD %d", data->current_speed);
    SDL_Rect speed_chip = {chip_x, chip_y, chip_width, chip_height};
    SDL_SetRenderDrawColor(renderer, 22, 32, 44, 255);
    SDL_RenderFillRect(renderer, &speed_chip);
    SDL_SetRenderDrawColor(renderer, 70, 110, 160, 255);
    SDL_RenderDrawRect(renderer, &speed_chip);
    draw_text(renderer, gui->button_font, speed_text, white, chip_x + 8, chip_y + 4);

    chip_x += chip_width + chip_spacing;
    char apples_text[24];
    snprintf(apples_text, sizeof(apples_text), "APP %d", data->apples_collected);
    SDL_Rect apples_chip = {chip_x, chip_y, chip_width, chip_height};
    SDL_SetRenderDrawColor(renderer, 22, 32, 44, 255);
    SDL_RenderFillRect(renderer, &apples_chip);
    SDL_SetRenderDrawColor(renderer, 70, 110, 160, 255);
    SDL_RenderDrawRect(renderer, &apples_chip);
    draw_text(renderer, gui->button_font, apples_text, white, chip_x + 8, chip_y + 4);

    chip_x += chip_width + chip_spacing;
    char len_text[24];
    snprintf(len_text, sizeof(len_text), "LEN %d", data->snake_length);
    SDL_Rect len_chip = {chip_x, chip_y, chip_width, chip_height};
    SDL_SetRenderDrawColor(renderer, 22, 32, 44, 255);
    SDL_RenderFillRect(renderer, &len_chip);
    SDL_SetRenderDrawColor(renderer, 70, 110, 160, 255);
    SDL_RenderDrawRect(renderer, &len_chip);
    draw_text(renderer, gui->button_font, len_text, white, chip_x + 8, chip_y + 4);

    if (data->mode == MODE_TIME_ATTACK)
    {
        int bar_width = gui_layout_scale(&gui->layout, 220);
        int bar_height = 12;
        SDL_Rect time_bg = {gui->layout.width - bar_width - 28, HEADER_HEIGHT - bar_height - 14, bar_width, bar_height};
        SDL_SetRenderDrawColor(renderer, 30, 40, 56, 255);
        SDL_RenderFillRect(renderer, &time_bg);
        SDL_Rect time_border = {time_bg.x, time_bg.y, time_bg.w, time_bg.h};
        SDL_SetRenderDrawColor(renderer, 70, 120, 190, 255);
        SDL_RenderDrawRect(renderer, &time_border);

        double progress = data->time_left / TIME_ATTACK_DURATION;
        if (progress < 0.0)
        {
            progress = 0.0;
        }
        if (progress > 1.0)
        {
            progress = 1.0;
        }
        SDL_Rect time_fg = {time_bg.x, time_bg.y, (int)(bar_width * progress), bar_height};
        SDL_SetRenderDrawColor(renderer, 120, 195, 255, 255);
        SDL_RenderFillRect(renderer, &time_fg);

        char timer_text[32];
        snprintf(timer_text, sizeof(timer_text), "TIME %.0fs", data->time_left);
        draw_text(renderer, gui->button_font, timer_text, white, time_bg.x + 8, time_bg.y - 20);
    }

    if (data->last_powerup_text[0] && data->current_time - data->last_powerup_display_time < 2.5)
    {
        GuiRect pickup = gui_make_rect(gui, 0.28f, 0.11f, 0.44f, 0.035f);
        SDL_Rect pickup_rect = {pickup.x, pickup.y, pickup.w, pickup.h};
        SDL_SetRenderDrawColor(renderer, 24, 34, 48, 230);
        SDL_RenderFillRect(renderer, &pickup_rect);
        SDL_SetRenderDrawColor(renderer, 80, 150, 220, 255);
        SDL_RenderDrawRect(renderer, &pickup_rect);
        draw_text(renderer, gui->button_font, data->last_powerup_text, accent, pickup.x + 12, pickup.y + 4);
    }

    if (data->active_powerup_count > 0)
    {
        int chip_width = gui_layout_scale(&gui->layout, 160);
        int chip_height = 22;
        int chip_spacing = 8;
        int base_x = gui->layout.width - 28 - chip_width;
        int base_y = 16;

        for (int i = 0; i < data->active_powerup_count; i++)
        {
            int row = i / 3;
            int col = i % 3;
            int power_x = base_x - col * (chip_width + chip_spacing);
            int power_y = base_y + row * (chip_height + 6);

            PowerupType type = data->active_powerups[i].type;
            PowerupInfo info = get_powerup_info(type);
            if (!info.duration || info.duration <= 0.0)
            {
                continue;
            }

            double powerup_time_left = data->active_powerups[i].end_time - data->current_time;
            if (powerup_time_left < 0.0)
            {
                powerup_time_left = 0.0;
            }

            char power_text[64];
            snprintf(power_text, sizeof(power_text), "%s %.1fs", info.name ? info.name : "UNKNOWN", powerup_time_left);
            SDL_Rect power_chip = {power_x, power_y, chip_width, chip_height};
            SDL_SetRenderDrawColor(renderer, 24, 34, 48, 255);
            SDL_RenderFillRect(renderer, &power_chip);
            SDL_SetRenderDrawColor(renderer, 80, 150, 220, 255);
            SDL_RenderDrawRect(renderer, &power_chip);
            draw_text(renderer, gui->button_font, power_text, accent, power_chip.x + 10, power_chip.y + 3);
        }
    }
}

void gui_hud_render_pause_overlay(GuiContext *gui)
{
    if (!gui || !gui->renderer)
    {
        return;
    }

    SDL_Renderer *renderer = gui->renderer;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect overlay = {0, GAME_AREA_TOP, gui->layout.width, GAME_AREA_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    draw_text(renderer, gui->large_font, "PAUSED", white, (gui->layout.width - 140) / 2, GAME_AREA_TOP + GAME_AREA_HEIGHT / 2 - 40);
}
