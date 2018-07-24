
#include "visuals/mainmenubutton.h"

MainmenuButton::MainmenuButton(std::string text_, double x_, double y_, std::function<void()> onclick_, sf::Font &font)
{
    text.setCharacterSize(NORMAL_BUTTON_FONT_SIZE);
    text.setFont(font);
    text.setPosition(x_, y_);
    text.setOrigin(0, HOVERED_BUTTON_FONT_SIZE);
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
        text.setOrigin(-30, HOVERED_BUTTON_FONT_SIZE);
    }
    else
    {
        text.setCharacterSize(NORMAL_BUTTON_FONT_SIZE);
        text.setOrigin(0, HOVERED_BUTTON_FONT_SIZE);
    }
    window.draw(text);
}