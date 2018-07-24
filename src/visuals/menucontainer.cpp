
#include "visuals/menucontainer.h"
#include "visuals/menu.h"
#include "visuals/mainmenu.h"
#include "visuals/lobbymenu.h"
#include "visuals/optionsmenu.h"

#include "global.h"

MenuContainer::MenuContainer(sf::RenderWindow &window) : planned_action(NOACTION), finished(false)
{
    current_menu = std::unique_ptr<Mainmenu>(new Mainmenu(window, *this));
}

MenuContainer::~MenuContainer()
{

}

bool MenuContainer::run(sf::RenderWindow &window)
{
    current_menu->run(window);
    if (action() == POSTMENUACTION::OPEN_LOBBY)
    {
        current_menu = std::unique_ptr<Menu>(new Lobbymenu(window, *this));
        planned_action = POSTMENUACTION::NOACTION;
    }
    else if (action() == POSTMENUACTION::OPEN_MAINMENU)
    {
        current_menu = std::unique_ptr<Mainmenu>(new Mainmenu(window, *this));
        planned_action = POSTMENUACTION::NOACTION;
    }
    else if (action() == POSTMENUACTION::OPEN_OPTIONS)
    {
        current_menu = std::unique_ptr<Optionsmenu>(new Optionsmenu(window, *this));
        planned_action = POSTMENUACTION::NOACTION;
    }

    if (finished)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
