#pragma once

#include "visuals/menucontainer.h"
#include "allegro5/allegro5.h"

class Menu
{
    public:
        Menu(ALLEGRO_DISPLAY *display, MenuContainer &owner_) : owner(owner_) {};
        virtual ~Menu() = default;
        virtual void run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue) = 0;
        virtual void quit() {owner.planned_action = POSTMENUACTION::QUIT; owner.exitmenus();}

        MenuContainer &owner;
};

