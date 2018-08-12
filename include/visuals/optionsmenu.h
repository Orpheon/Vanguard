#pragma once

#include "spriteloader.h"
#include "visuals/menu.h"
#include "visuals/menuanimation.h"
#include "visuals/menucontainer.h"
#include "allegro5/allegro5.h"

class Optionsmenu : public Menu
{
    public:
        Optionsmenu(sf::RenderWindow &window, MenuContainer &owner_);
        virtual ~Optionsmenu() = default;
        void run(sf::RenderWindow &window) override;
        void openmainmenu();

        Spriteloader spriteloader;
        MenuLoopAnimation background;
    protected:
    private:
};
