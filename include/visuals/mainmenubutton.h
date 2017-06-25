#pragma once

#include "datastructures.h"

#include <string>
#include <functional>
#include "allegro5/allegro5.h"
#include "allegro5/allegro_font.h"

class MainmenuButton
{
    public:
        MainmenuButton(std::string text_, double x_, double y_, std::function<void()> onclick_,
                       ALLEGRO_FONT *normal_font_, ALLEGRO_FONT *hovered_font_);
        void render(ALLEGRO_DISPLAY *display, double mouse_x, double mouse_y);
        bool ontop(double mouse_x, double mouse_y);
        std::function<void()> onclick;
    private:
        std::string text;
        ALLEGRO_FONT *normal_font;
        ALLEGRO_FONT *hovered_font;
        Rect bbox;
};