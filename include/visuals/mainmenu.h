#pragma once

#include "spriteloader.h"
#include "visuals/menuanimation.h"
#include "visuals/mainmenubutton.h"
#include "allegro5/allegro5.h"

#include <list>
#include <memory>

class Mainmenu
{
    public:
        Mainmenu(ALLEGRO_DISPLAY *display);
        virtual ~Mainmenu();
        bool run(ALLEGRO_DISPLAY *display);
        Spriteloader spriteloader;
        ALLEGRO_EVENT_QUEUE *event_queue;
        MenuLoopAnimation background;
    protected:
    private:
        std::list<std::unique_ptr<MainmenuButton>> buttons;
};

