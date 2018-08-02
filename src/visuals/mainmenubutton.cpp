
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "visuals/mainmenubutton.h"

MainmenuButton::MainmenuButton(std::string text_, double x_, double y_, std::function<void()> onclick_, sf::Font &font_)
{
    font = font_;
    text.setCharacterSize(NORMAL_BUTTON_FONT_SIZE);
    text.setFont(font);
    text.setPosition(x_, y_);
    text.setString(text_);
    onclick = onclick_;
    bbox = Rect(x_, y_, 350, HOVERED_BUTTON_FONT_SIZE);
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

void MainmenuButton::render(sf::RenderWindow &window, double mouse_x, double mouse_y)
{
    if (ontop(mouse_x, mouse_y))
    {
        // We're being hovered over
        text.setCharacterSize(HOVERED_BUTTON_FONT_SIZE);
        text.setOrigin(-30, 0);
    }
    else
    {
        text.setCharacterSize(NORMAL_BUTTON_FONT_SIZE);
        text.setOrigin(0, 0);
    }
    text.setFillColor(sf::Color::White);
    window.draw(text);
}