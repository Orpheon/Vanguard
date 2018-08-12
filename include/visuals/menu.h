#pragma once

#include "visuals/menucontainer.h"

class Menu
{
    public:
        Menu(sf::RenderWindow &window, MenuContainer &owner_) : owner(owner_) {};
        virtual ~Menu() = default;
        virtual void run(sf::RenderWindow &window) = 0;
        virtual void quit() {owner.planned_action = POSTMENUACTION::QUIT; owner.exitmenus();}

        MenuContainer &owner;
};

