#ifndef MAINMENU_H
#define MAINMENU_H

#include "spriteloader.h"
#include "allegro5/allegro5.h"

class MainMenu
{
    public:
        MainMenu(ALLEGRO_DISPLAY *display);
        virtual ~MainMenu();
        bool run(ALLEGRO_DISPLAY *display, GAMETYPE *gametype);
        Spriteloader spriteloader;
        ALLEGRO_EVENT_QUEUE *event_queue;
    protected:
    private:
};

#endif // MAINMENU_H
