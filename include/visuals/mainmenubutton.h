#pragma once

#include "datastructures.h"

#include <string>
#include <functional>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class MainmenuButton
{
    public:
        MainmenuButton(std::string text_, double x_, double y_, std::function<void()> onclick_, sf::Font &font);
        void render(sf::RenderWindow &window, double mouse_x, double mouse_y);
        bool ontop(double mouse_x, double mouse_y);
        std::function<void()> onclick;
    private:
        sf::Text text;
        Rect bbox;
        int NORMAL_BUTTON_FONT_SIZE = 30;
        int HOVERED_BUTTON_FONT_SIZE = 40;
};