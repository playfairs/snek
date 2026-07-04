#include "gui/gui.h"
#include "gui/hud.h"
#include "gui/layout.h"
#include "gui/menu.h"

void gui_init(GuiContext *gui,
              SDL_Renderer *renderer,
              TTF_Font *font,
              TTF_Font *button_font,
              TTF_Font *score_font,
              TTF_Font *large_font)
{
    if (!gui)
    {
        return;
    }

    gui->renderer = renderer;
    gui->font = font;
    gui->button_font = button_font;
    gui->score_font = score_font;
    gui->large_font = large_font;
    gui_layout_update(&gui->layout, DIS_WIDTH, DIS_HEIGHT);
}

void gui_cleanup(GuiContext *gui)
{
    (void)gui;
}

void gui_refresh_layout(GuiContext *gui)
{
    if (!gui || !gui->renderer)
    {
        return;
    }

    int width = 0;
    int height = 0;
    SDL_GetRendererOutputSize(gui->renderer,
                              &width,
                              &height);
    gui_layout_update(&gui->layout, width, height);
}

GuiRect gui_make_rect(const GuiContext *gui,
                      float x_ratio,
                      float y_ratio,
                      float w_ratio,
                      float h_ratio)
{
    return gui_layout_rect(&gui->layout,
                           x_ratio,
                           y_ratio,
                           w_ratio,
                           h_ratio);
}

void gui_render_main_menu(GuiContext *gui,
                          SnakeSkin current_skin,
                          const GameStats *stats)
{
    if (!gui)
    {
        return;
    }

    gui_refresh_layout(gui);
    gui_menu_render_main_menu(gui, current_skin, stats);
}

void gui_render_settings_menu(GuiContext *gui,
                              const GameSettings *settings,
                              const GameStats *stats)
{
    if (!gui)
    {
        return;
    }

    gui_refresh_layout(gui);
    gui_menu_render_settings_menu(gui, settings, stats);
}

void gui_render_stats_menu(GuiContext *gui,
                           const GameStats *stats)
{
    if (!gui)
    {
        return;
    }

    gui_refresh_layout(gui);
    gui_menu_render_stats_menu(gui, stats);
}

void gui_render_game_over(GuiContext *gui, int score)
{
    if (!gui)
    {
        return;
    }

    gui_refresh_layout(gui);
    gui_menu_render_game_over(gui, score);
}

void gui_render_hud(GuiContext *gui, const GuiHudData *data)
{
    if (!gui || !data)
    {
        return;
    }

    gui_refresh_layout(gui);
    gui_hud_render(gui, data);
}

void gui_render_pause_overlay(GuiContext *gui, int paused)
{
    if (!gui || !paused)
    {
        return;
    }

    gui_refresh_layout(gui);
    gui_hud_render_pause_overlay(gui);
}
