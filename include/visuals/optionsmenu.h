#pragma once

#include "spriteloader.h"
#include "visuals/menu.h"
#include "visuals/menuanimation.h"
#include "visuals/menucontainer.h"
#include "allegro5/allegro5.h"

class Optionsmenu : public Menu
{
    public:
        Optionsmenu(ALLEGRO_DISPLAY *display, MenuContainer &owner_);
        virtual ~Optionsmenu() = default;
        void run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue) override;

        Spriteloader spriteloader;
        MenuLoopAnimation background;
    protected:
    private:
};
