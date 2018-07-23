#pragma once

#include "allegro5/allegro5.h"
#include "datastructures.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

class Menu;

class MenuContainer
{
    public:
        MenuContainer(sf::RenderWindow &window);
        ~MenuContainer();
        bool run(sf::RenderWindow &window);
        void exitmenus() { finished = true; }
        int action() { return planned_action; }

        int planned_action;
        std::string serverip;
        int serverport;

    private:
        ALLEGRO_EVENT_QUEUE *event_queue;
        std::unique_ptr<Menu> current_menu;
        bool finished;
};