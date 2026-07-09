#ifndef GUI_LAYOUT_H
#define GUI_LAYOUT_H

#include "gui/gui.h"

void gui_layout_update(GuiLayout *layout,
                       int width,
                       int height);
GuiRect gui_layout_rect(const GuiLayout *layout,
                        float x_ratio,
                        float y_ratio,
                        float w_ratio,
                        float h_ratio);
int gui_layout_scale(const GuiLayout *layout, int value);

#endif
