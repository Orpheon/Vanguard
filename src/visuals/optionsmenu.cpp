
#include <SFML/Window/Event.hpp>
#include "visuals/optionsmenu.h"
#include "global_constants.h"

Optionsmenu::Optionsmenu(sf::RenderWindow &window, MenuContainer &owner_) : Menu(window, owner_)
{
    background.init("ui/menus/optionsmenu/");
}

void Optionsmenu::run(sf::RenderWindow &window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                quit();
                break;

            case sf::Event::Resized:
                // Nothing - probably placeholder
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                {
                    openmainmenu();
                }
                break;

            default:
                break;
        }
    }

    background.update(MENU_TIMESTEP);

    sf::Vector2i mousepos = sf::Mouse::getPosition(window);
    sf::Vector2u windowsize = window.getSize();

    // Draw
    int WINDOW_WIDTH = windowsize.x;
    int WINDOW_HEIGHT = windowsize.y;
    sf::Sprite bgsprite;
    spriteloader.loadsprite(background.getframepath(), bgsprite);
    sf::FloatRect size = bgsprite.getLocalBounds();
    bgsprite.setPosition(0, 0);
    bgsprite.setScale(WINDOW_WIDTH/size.width, WINDOW_HEIGHT/size.height);
    bgsprite.setOrigin(0, 0);
    window.draw(bgsprite);

    window.display();
}

void Optionsmenu::openmainmenu()
{
    owner.planned_action = POSTMENUACTION::OPEN_MAINMENU;
}