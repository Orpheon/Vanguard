#pragma once

#include "allegro5/allegro5.h"
#include "datastructures.h"

#include <memory>

class Menu;

class MenuContainer
{
    public:
        MenuContainer(ALLEGRO_DISPLAY *display);
        ~MenuContainer();
        bool run(ALLEGRO_DISPLAY *display);
        void exitmenus() { finished = true; }
        int action() { return planned_action; }

        int planned_action;

    private:
        ALLEGRO_EVENT_QUEUE *event_queue;
        std::unique_ptr<Menu> current_menu;
        bool finished;
};