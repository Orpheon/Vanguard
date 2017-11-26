
#include "visuals/lobbymenu.h"
#include "global_constants.h"

Lobbymenu::Lobbymenu(ALLEGRO_DISPLAY *display, MenuContainer &owner_) : Menu(display, owner_), spriteloader(false)
{
    background.init("ui/Menu/background/");
}

void Lobbymenu::run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue)
{
    ALLEGRO_EVENT event;
    // Capture events first
    while (al_get_next_event(event_queue, &event))
    {
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            // Deliberate closing, not an error
            quit();
        }

        switch (event.type)
        {
            case ALLEGRO_EVENT_KEY_CHAR:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    // Go back to main menu
                    owner.planned_action = POSTMENUACTION::OPEN_MAINMENU;
                }
        }
    }

    background.update(MENU_TIMESTEP);

    al_draw_bitmap(spriteloader.requestsprite(background.getframepath()), 0, 0, 0);
    al_flip_display();
}

void Lobbymenu::quit()
{
    owner.planned_action = POSTMENUACTION::QUIT;
    owner.exitmenus();
}