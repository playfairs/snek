#ifndef GUI_MENU_H
#define GUI_MENU_H

#include "gui/gui.h"

void gui_menu_render_main_menu(GuiContext *gui,
                               SnakeSkin current_skin,
                               const GameStats *stats);
void gui_menu_render_settings_menu(
    GuiContext *gui,
    const GameSettings *settings,
    const GameStats *stats);
void gui_menu_render_stats_menu(GuiContext *gui,
                                const GameStats *stats);
void gui_menu_render_game_over(GuiContext *gui, int score);

#endif
