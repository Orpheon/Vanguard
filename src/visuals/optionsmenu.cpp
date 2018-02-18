
#include "visuals/optionsmenu.h"
#include "global_constants.h"

Optionsmenu::Optionsmenu(ALLEGRO_DISPLAY *display, MenuContainer &owner_) : Menu(display, owner_), spriteloader(false)
{
    background.init("ui/menus/optionsmenu/");
}

void Optionsmenu::run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue)
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

        if (event.type ==  ALLEGRO_EVENT_DISPLAY_RESIZE)
        {
            al_acknowledge_resize(display);
        }

        if (event.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                openmainmenu();
            }
        }
    }

    background.update(MENU_TIMESTEP);

    int WINDOW_WIDTH = al_get_display_width(display);
    int WINDOW_HEIGHT = al_get_display_height(display);
    ALLEGRO_BITMAP* bgsprite = spriteloader.requestsprite(background.getframepath());
    double w = al_get_bitmap_width(bgsprite), h = al_get_bitmap_height(bgsprite);
    al_draw_scaled_bitmap(bgsprite, 0, 0, w, h, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    al_flip_display();
}

void Optionsmenu::openmainmenu()
{
    owner.planned_action = POSTMENUACTION::OPEN_MAINMENU;
}