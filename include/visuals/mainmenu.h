#pragma once

#include "spriteloader.h"
#include "visuals/menu.h"
#include "visuals/menuanimation.h"
#include "visuals/mainmenubutton.h"
#include "visuals/menucontainer.h"
#include "allegro5/allegro5.h"

#include <list>
#include <memory>

class Mainmenu : public Menu
{
    public:
        Mainmenu(sf::RenderWindow &window, MenuContainer &owner_);
        virtual ~Mainmenu() = default;
        void run(sf::RenderWindow &window) override;
        void hostserver();
        void joinlobby();
        void joinself();
        void connectmanually();
        void openoptions();

        Spriteloader spriteloader;
        MenuLoopAnimation background;
    protected:
    private:
        std::list<std::unique_ptr<MainmenuButton>> buttons;

        bool istypingIP;
        std::string ipstring;
        sf::Font textfont;
};
