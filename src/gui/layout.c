#include "gui/layout.h"

void gui_layout_update(GuiLayout *layout,
                       int width,
                       int height)
{
    if (!layout)
    {
        return;
    }

    layout->width = width > 0 ? width : DIS_WIDTH;
    layout->height = height > 0 ? height : DIS_HEIGHT;
}

GuiRect gui_layout_rect(const GuiLayout *layout,
                        float x_ratio,
                        float y_ratio,
                        float w_ratio,
                        float h_ratio)
{
    GuiRect rect;
    if (!layout)
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = DIS_WIDTH;
        rect.h = DIS_HEIGHT;
        return rect;
    }

    rect.x = (int)(layout->width * x_ratio);
    rect.y = (int)(layout->height * y_ratio);
    rect.w = (int)(layout->width * w_ratio);
    rect.h = (int)(layout->height * h_ratio);
    return rect;
}

int gui_layout_scale(const GuiLayout *layout, int value)
{
    if (!layout || layout->width <= 0)
    {
        return value;
    }

    return (int)((value * layout->width) / DIS_WIDTH);
}
