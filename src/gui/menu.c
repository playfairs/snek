#include "gui/menu.h"
#include "core/game/constants.h"
#include "gui/layout.h"
#include "core/graphics/graphics.h"
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

static void draw_preview_grid(SDL_Renderer *renderer,
                              const GuiRect *rect)
{
    SDL_Rect box = {rect->x, rect->y, rect->w, rect->h};
    SDL_SetRenderDrawColor(renderer, 17, 25, 34, 255);
    SDL_RenderFillRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, 49, 73, 87, 255);
    SDL_RenderDrawRect(renderer, &box);
    SDL_SetRenderDrawColor(renderer, 28, 40, 52, 255);
    
    int grid_start_x = rect->x;
    int grid_start_y = rect->y;
    
    for (int x = grid_start_x; x <= rect->x + rect->w; x += SNAKE_BLOCK)
    {
        SDL_RenderDrawLine(renderer, x, rect->y, x, rect->y + rect->h);
    }
    for (int y = grid_start_y; y <= rect->y + rect->h; y += SNAKE_BLOCK)
    {
        SDL_RenderDrawLine(renderer, rect->x, y, rect->x + rect->w, y);
    }
}

static void draw_snake_preview(SDL_Renderer *renderer,
                               SnakeSkin skin,
                               const GuiRect *rect,
                               int length)
{
    if (!rect)
        return;

    if (length <= 0)
    {
        int max_blocks = rect->w / SNAKE_BLOCK;
        length = max_blocks - 2;
        if (length < 4)
            length = 4;
        if (length > 14)
            length = 14;
    }

    if (length % 2 != 0)
        length++;

    Snake preview_snake = {0};
    preview_snake.length = length;
    preview_snake.dx = 0;
    preview_snake.dy = 0;
    preview_snake.skin = skin;

    int grid_cols = rect->w / SNAKE_BLOCK;
    int grid_rows = rect->h / SNAKE_BLOCK;
    
    int start_col = (grid_cols - length) / 2;
    int start_row = grid_rows / 2;
    
    int start_x = rect->x + start_col * SNAKE_BLOCK;
    int start_y = rect->y + start_row * SNAKE_BLOCK;

    for (int i = 0; i < length; i++)
    {
        preview_snake.segments[i].x = start_x + i * SNAKE_BLOCK;
        preview_snake.segments[i].y = start_y;
    }

    draw_snake(renderer, &preview_snake, 0, 0);
}

int gui_draw_button(GuiContext *gui,
                    const char *text,
                    int x,
                    int y,
                    int w,
                    int h,
                    int mouse_x,
                    int mouse_y,
                    int *clicked)
{
    if (!gui || !gui->renderer || !text || !clicked)
    {
        return 0;
    }

    SDL_Renderer *renderer = gui->renderer;
    SDL_Rect rect = {x, y, w, h};
    SDL_Rect shadow = {x + 4, y + 5, w, h};

    int is_hover = (mouse_x >= x && mouse_x <= x + w
                    && mouse_y >= y && mouse_y <= y + h);
    int is_pressed = is_hover && *clicked;

    SDL_Color bg_color = is_pressed ? (SDL_Color){50, 85, 120, 255}
                        : is_hover ? (SDL_Color){60, 100, 140, 255}
                                   : (SDL_Color){28, 42, 55, 255};
    SDL_Color border_color = is_pressed ? (SDL_Color){90, 150, 200, 255}
                           : is_hover ? (SDL_Color){110, 170, 230, 255}
                                      : (SDL_Color){62, 86, 98, 255};

    SDL_SetRenderDrawColor(renderer, 5, 8, 12, 255);
    SDL_RenderFillRect(renderer, &shadow);
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, 255);
    SDL_RenderDrawRect(renderer, &rect);

    SDL_Color accent = {124, 218, 184, 255};
    SDL_Rect accent_strip = {x, y, 5, h};
    SDL_SetRenderDrawColor(renderer, accent.r, accent.g, accent.b, 255);
    SDL_RenderFillRect(renderer, &accent_strip);

    SDL_Color white = {WHITE_R, WHITE_G, WHITE_B, 255};
    SDL_Surface *text_surf = TTF_RenderText_Blended(gui->button_font, text, white);
    if (text_surf)
    {
        SDL_Texture *text_tex = SDL_CreateTextureFromSurface(renderer, text_surf);
        if (text_tex)
        {
            SDL_Rect text_rect = {x + (w - text_surf->w) / 2,
                                  y + (h - text_surf->h) / 2,
                                  text_surf->w,
                                  text_surf->h};
            SDL_RenderCopy(renderer, text_tex, NULL, &text_rect);
            SDL_DestroyTexture(text_tex);
        }
        SDL_FreeSurface(text_surf);
    }

    int result = is_hover && *clicked;
    if (result)
    {
        *clicked = 0;
    }
    return result;
}

void gui_menu_render_main_menu(GuiContext *gui,
                               SnakeSkin current_skin,
                               const GameStats *stats)
{
    if (!gui || !gui->renderer)
    {
        return;
    }

    SDL_Renderer *renderer = gui->renderer;
    SDL_SetRenderDrawColor(renderer, 9, 12, 18, 255);
    SDL_RenderClear(renderer);

    GuiRect shell = gui_make_rect(gui, 0.08f, 0.08f, 0.84f, 0.84f);
    draw_panel(renderer, &shell, (SDL_Color){16, 22, 31, 255}, (SDL_Color){58, 83, 98, 255});
    
    draw_text(renderer, gui->large_font, "SNEK", (SDL_Color){124, 218, 184, 255}, shell.x + 24, shell.y + 20);
    draw_text(renderer, gui->button_font, "Fast, clean, bite-sized arcade survival.", (SDL_Color){158, 170, 184, 255}, shell.x + 24, shell.y + 56);

    int left_col_w = (int)(shell.w * 0.55f);
    int left_col_x = shell.x + (int)(shell.w * 0.03f);
    
    int preview_h = (int)(shell.h * 0.55f);
    GuiRect preview_panel = {
        left_col_x,
        shell.y + (int)(shell.h * 0.15f),
        left_col_w,
        preview_h};
    draw_panel(renderer, &preview_panel, (SDL_Color){18, 26, 35, 255}, (SDL_Color){58, 83, 98, 255});
    draw_text(renderer, gui->button_font, "CURRENT SKIN", (SDL_Color){255, 196, 92, 255}, preview_panel.x + 16, preview_panel.y + 12);
    draw_preview_grid(renderer, &preview_panel);
    draw_snake_preview(renderer, current_skin, &preview_panel, 0);

    Food preview_food = {
        {preview_panel.x + SNAKE_BLOCK * 3, 
         preview_panel.y + SNAKE_BLOCK * 4}, 
        ITEM_APPLE, 1, 0.0, 0.0};
    
    Powerup preview_powerup = {
        {preview_panel.x + SNAKE_BLOCK * 7, 
         preview_panel.y + SNAKE_BLOCK * 6}, 
        POWERUP_SPEED_BOOST, 1, 0.0};
    
    draw_food(renderer, &preview_food);
    draw_powerup(renderer, &preview_powerup);

    int stats_h = (int)(shell.h * 0.25f);
    GuiRect stats_panel = {
        left_col_x,
        preview_panel.y + preview_panel.h + 12,
        left_col_w,
        stats_h};
    draw_panel(renderer, &stats_panel, (SDL_Color){18, 26, 35, 255}, (SDL_Color){58, 83, 98, 255});
    draw_text(renderer, gui->button_font, "STATISTICS", (SDL_Color){255, 196, 92, 255}, stats_panel.x + 16, stats_panel.y + 12);
    
    if (stats) {
        char high_score_text[64];
        snprintf(high_score_text, sizeof(high_score_text), "High Score: %d", stats->high_score);
        draw_text(renderer, gui->button_font, high_score_text, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, stats_panel.x + 16, stats_panel.y + 40);
        
        char skins_text[64];
        snprintf(skins_text, sizeof(skins_text), "Skins: %d", SKIN_COUNT);
        draw_text(renderer, gui->button_font, skins_text, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, stats_panel.x + 16, stats_panel.y + 64);
        
        char powerups_text[64];
        snprintf(powerups_text, sizeof(powerups_text), "Powerups: %d", POWERUP_NONE);
        draw_text(renderer, gui->button_font, powerups_text, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, stats_panel.x + 16, stats_panel.y + 88);
    }

    int right_col_x = left_col_x + left_col_w + 20;
    int right_col_w = shell.w - left_col_w - (int)(shell.w * 0.06f) - 20;
    
    GuiRect action_panel = {
        right_col_x,
        shell.y + (int)(shell.h * 0.15f),
        right_col_w,
        shell.h - (int)(shell.h * 0.15f) - (int)(shell.h * 0.05f)};
    draw_panel(renderer, &action_panel, (SDL_Color){20, 29, 39, 255}, (SDL_Color){69, 92, 105, 255});
    draw_text(renderer, gui->button_font, "MAIN MENU", (SDL_Color){255, 196, 92, 255}, action_panel.x + 20, action_panel.y + 20);
    draw_text(renderer, gui->button_font, "WASD or arrows in game", (SDL_Color){158, 170, 184, 255}, action_panel.x + 20, action_panel.y + 56);
    draw_text(renderer, gui->button_font, "ESC pauses or backs out", (SDL_Color){158, 170, 184, 255}, action_panel.x + 20, action_panel.y + 80);
}

void gui_menu_render_settings_menu(GuiContext *gui,
                                   const GameSettings *settings,
                                   const GameStats *stats)
{
    if (!gui || !gui->renderer || !settings || !stats)
    {
        return;
    }

    SDL_Renderer *renderer = gui->renderer;
    SDL_SetRenderDrawColor(renderer, 8, 11, 17, 255);
    SDL_RenderClear(renderer);

    GuiRect frame = gui_make_rect(gui, 0.08f, 0.08f, 0.84f, 0.84f);
    draw_panel(renderer, &frame, (SDL_Color){15, 21, 30, 255}, (SDL_Color){57, 80, 95, 255});

    draw_text(renderer, gui->large_font, "SETTINGS", (SDL_Color){124, 218, 184, 255}, frame.x + 24, frame.y + 20);
    draw_text(renderer, gui->button_font, "Changes save immediately.", (SDL_Color){158, 170, 184, 255}, frame.x + 26, frame.y + 56);

    const char *skin_names[] = {"Default", "Blue", "Red", "Rainbow", "Purple", "Gold", "Cyan", "Neon", "Holographic", "Monochrome", "Chromatic", "Cosmic", "Chrome", "Bubble", "Shadow", "Solar", "Aqua", "Midnight"};
    const char *mode_names[] = {"Classic", "Challenge", "Time Attack", "Endless", "Berserk", "Maze", "Survival", "Ghost Run", "Inverse", "Rainbow Run"};
    const char *difficulty_names[] = {"Easy", "Normal", "Hard"};

    int left_col_w = (int)(frame.w * 0.50f);
    int left_col_x = frame.x + (int)(frame.w * 0.03f);
    
    int preview_h = (int)(frame.h * 0.45f);
    GuiRect preview_panel = {
        left_col_x,
        frame.y + (int)(frame.h * 0.13f),
        left_col_w,
        preview_h};
    draw_panel(renderer, &preview_panel, (SDL_Color){18, 26, 35, 255}, (SDL_Color){58, 83, 98, 255});
    draw_text(renderer, gui->button_font, "SKIN PREVIEW", (SDL_Color){255, 196, 92, 255}, preview_panel.x + 16, preview_panel.y + 12);
    draw_preview_grid(renderer, &preview_panel);
    draw_snake_preview(renderer, settings->current_skin, &preview_panel, 0);

    Food preview_food = {
        {preview_panel.x + SNAKE_BLOCK * 3, 
         preview_panel.y + SNAKE_BLOCK * 4}, 
        ITEM_APPLE, 1, 0.0, 0.0};
    
    Powerup preview_powerup = {
        {preview_panel.x + SNAKE_BLOCK * 7, 
         preview_panel.y + SNAKE_BLOCK * 6}, 
        POWERUP_SPEED_BOOST, 1, 0.0};
    
    draw_food(renderer, &preview_food);
    draw_powerup(renderer, &preview_powerup);

    int stats_h = (int)(frame.h * 0.32f);
    GuiRect stats_panel = {
        left_col_x,
        preview_panel.y + preview_panel.h + 12,
        left_col_w,
        stats_h};
    draw_panel(renderer, &stats_panel, (SDL_Color){18, 26, 35, 255}, (SDL_Color){58, 83, 98, 255});
    draw_text(renderer, gui->button_font, "STATISTICS", (SDL_Color){255, 196, 92, 255}, stats_panel.x + 16, stats_panel.y + 12);
    
    char high_score_text[64];
    snprintf(high_score_text, sizeof(high_score_text), "High Score: %d", stats->high_score);
    draw_text(renderer, gui->button_font, high_score_text, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, stats_panel.x + 16, stats_panel.y + 40);
    
    char skins_text[64];
    snprintf(skins_text, sizeof(skins_text), "Skins: %d", SKIN_COUNT);
    draw_text(renderer, gui->button_font, skins_text, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, stats_panel.x + 16, stats_panel.y + 64);
    
    char powerups_text[64];
    snprintf(powerups_text, sizeof(powerups_text), "Powerups: %d", POWERUP_NONE);
    draw_text(renderer, gui->button_font, powerups_text, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, stats_panel.x + 16, stats_panel.y + 88);

    int right_col_x = left_col_x + left_col_w + 20;
    int right_col_w = frame.w - left_col_w - (int)(frame.w * 0.06f) - 20;
    
    GuiRect settings_panel = {
        right_col_x,
        frame.y + (int)(frame.h * 0.13f),
        right_col_w,
        frame.h - (int)(frame.h * 0.13f) - (int)(frame.h * 0.05f)};
    draw_panel(renderer, &settings_panel, (SDL_Color){20, 29, 39, 255}, (SDL_Color){69, 92, 105, 255});
    draw_text(renderer, gui->button_font, "GAME SETTINGS", (SDL_Color){255, 196, 92, 255}, settings_panel.x + 20, settings_panel.y + 20);
}

void gui_menu_render_stats_menu(GuiContext *gui,
                                const GameStats *stats)
{
    if (!gui || !gui->renderer || !stats)
    {
        return;
    }

    SDL_Renderer *renderer = gui->renderer;
    SDL_SetRenderDrawColor(renderer, 10, 14, 22, 255);
    SDL_RenderClear(renderer);

    GuiRect panel = gui_make_rect(gui, 0.08f, 0.11f, 0.84f, 0.74f);
    draw_panel(renderer, &panel, (SDL_Color){22, 28, 40, 255}, (SDL_Color){80, 120, 170, 255});
    draw_text(renderer, gui->large_font, "STATISTICS", (SDL_Color){150, 220, 255, 255}, panel.x + 24, panel.y + 24);

    int y = panel.y + 90;
    char buf[64];
    snprintf(buf, sizeof(buf), "High Score: %d", stats->high_score);
    draw_text(renderer, gui->score_font, buf, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, panel.x + 24, y);
    y += 36;
    snprintf(buf, sizeof(buf), "Total Apples: %d", stats->total_apples);
    draw_text(renderer, gui->score_font, buf, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, panel.x + 24, y);
    y += 36;
    snprintf(buf, sizeof(buf), "Games Played: %d", stats->games_played);
    draw_text(renderer, gui->score_font, buf, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, panel.x + 24, y);
    y += 36;
    snprintf(buf, sizeof(buf), "Powerups Collected: %d", stats->powerups_collected);
    draw_text(renderer, gui->score_font, buf, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, panel.x + 24, y);
    y += 36;
    snprintf(buf, sizeof(buf), "Best Time: %.1fs", stats->best_time);
    draw_text(renderer, gui->score_font, buf, (SDL_Color){170, 180, 190, 255}, panel.x + 24, y);
}

void gui_menu_render_game_over(GuiContext *gui, int score)
{
    if (!gui || !gui->renderer)
    {
        return;
    }

    SDL_Renderer *renderer = gui->renderer;
    SDL_SetRenderDrawColor(renderer, 10, 14, 22, 255);
    SDL_RenderClear(renderer);

    GuiRect card = gui_make_rect(gui, 0.18f, 0.21f, 0.64f, 0.42f);
    draw_panel(renderer, &card, (SDL_Color){24, 32, 46, 255}, (SDL_Color){90, 140, 210, 255});
    draw_text(renderer, gui->large_font, "GAME OVER", (SDL_Color){150, 220, 255, 255}, card.x + 24, card.y + 24);

    char score_text[32];
    snprintf(score_text, sizeof(score_text), "Final Score: %d", score);
    draw_text(renderer, gui->score_font, score_text, (SDL_Color){WHITE_R, WHITE_G, WHITE_B, 255}, card.x + 24, card.y + 90);
    draw_text(renderer, gui->button_font, "Hit PLAY AGAIN to try for a new high score.", (SDL_Color){170, 180, 190, 255}, card.x + 24, card.y + 130);
}
