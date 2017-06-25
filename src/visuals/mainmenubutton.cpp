
#include "visuals/mainmenubutton.h"

MainmenuButton::MainmenuButton(std::string text_, double x_, double y_, std::function<void()> onclick_,
                               ALLEGRO_FONT *normal_font_, ALLEGRO_FONT *hovered_font_)
{
    text = text_;
    onclick = onclick_;
    normal_font = normal_font_;
    hovered_font = hovered_font_;
    bbox = Rect(x_, y_, 350, al_get_font_line_height(hovered_font));
}

bool MainmenuButton::ontop(double mouse_x, double mouse_y)
{
    if (mouse_x >= bbox.x and mouse_x <= bbox.x + bbox.w)
    {
        if (mouse_y >= bbox.y and mouse_y <= bbox.y + bbox.h)
        {
            return true;
        }
    }
    return false;
}

void MainmenuButton::render(ALLEGRO_DISPLAY *display, double mouse_x, double mouse_y)
{
    ALLEGRO_FONT *current_font = normal_font;
    int offset = 0;
    if (ontop(mouse_x, mouse_y))
    {
        // We're being hovered over
        current_font = hovered_font;
        offset = 30;
    }

    al_draw_text(current_font, al_map_rgb(255, 255, 255), bbox.x + offset, bbox.y, ALLEGRO_ALIGN_LEFT, text.c_str());
}