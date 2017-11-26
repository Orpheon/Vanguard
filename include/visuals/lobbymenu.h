#pragma once

#include "visuals/menu.h"
#include "visuals/menuanimation.h"
#include "spriteloader.h"

class Lobbymenu : public Menu
{
    public:
        Lobbymenu(ALLEGRO_DISPLAY *display, MenuContainer &owner_);
        virtual ~Lobbymenu() = default;
        void run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue) override;

        void quit();

        MenuLoopAnimation background;
        Spriteloader spriteloader;
    protected:
    private:
};
