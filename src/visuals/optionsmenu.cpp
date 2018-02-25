
#include "visuals/optionsmenu.h"
#include "global_constants.h"

Optionsmenu::Optionsmenu(ALLEGRO_DISPLAY *display, MenuContainer &owner_) : Menu(display, owner_), spriteloader(false)
{
    background.init("ui/menus/optionsmenu/");
}

void Optionsmenu::run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue)
{
    background.update(MENU_TIMESTEP);

    al_draw_bitmap(spriteloader.requestsprite(background.getframepath()), 0, 0, 0);
    al_flip_display();
}