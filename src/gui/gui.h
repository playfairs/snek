#ifndef GUI_H
#define GUI_H

#include "core/game/game.h"
#include "core/game/state.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct
{
    int width;
    int height;
} GuiLayout;

typedef struct
{
    int x;
    int y;
    int w;
    int h;
} GuiRect;

struct GuiContext
{
    SDL_Renderer *renderer;
    TTF_Font *font;
    TTF_Font *button_font;
    TTF_Font *score_font;
    TTF_Font *large_font;
    GuiLayout layout;
};

typedef struct
{
    int score;
    int high_score;
    int snake_length;
    int apples_collected;
    int current_speed;
    int active_powerup_count;
    ActivePowerup active_powerups[MAX_POWERUPS];
    GameMode mode;
    double time_left;
    char last_powerup_text[64];
    double last_powerup_display_time;
    double current_time;
    int paused;
} GuiHudData;

void gui_init(GuiContext *gui,
              SDL_Renderer *renderer,
              TTF_Font *font,
              TTF_Font *button_font,
              TTF_Font *score_font,
              TTF_Font *large_font);
void gui_cleanup(GuiContext *gui);
void gui_refresh_layout(GuiContext *gui);
GuiRect gui_make_rect(const GuiContext *gui,
                      float x_ratio,
                      float y_ratio,
                      float w_ratio,
                      float h_ratio);
void gui_render_main_menu(GuiContext *gui,
                          SnakeSkin current_skin,
                          const GameStats *stats);
void gui_render_settings_menu(GuiContext *gui,
                              const GameSettings *settings,
                              const GameStats *stats);
void gui_render_stats_menu(GuiContext *gui,
                           const GameStats *stats);
void gui_render_game_over(GuiContext *gui, int score);
void gui_render_hud(GuiContext *gui,
                    const GuiHudData *data);
void gui_render_pause_overlay(GuiContext *gui, int paused);
int gui_draw_button(GuiContext *gui,
                    const char *text,
                    int x,
                    int y,
                    int w,
                    int h,
                    int mouse_x,
                    int mouse_y,
                    int *clicked);

#endif
