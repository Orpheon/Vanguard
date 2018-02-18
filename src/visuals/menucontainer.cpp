
#include "visuals/menucontainer.h"
#include "visuals/menu.h"
#include "visuals/mainmenu.h"
#include "visuals/lobbymenu.h"
#include "visuals/optionsmenu.h"

#include "global.h"

MenuContainer::MenuContainer(ALLEGRO_DISPLAY *display) : planned_action(NOACTION), finished(false)
{
    // Create an event queue, and error if it fails
    event_queue = al_create_event_queue();
    if (!event_queue)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not create event queue");
    }

    // Connect the window, keyboard and mouse events to this event queue
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    current_menu = std::unique_ptr<Mainmenu>(new Mainmenu(display, *this));
}

MenuContainer::~MenuContainer()
{
    al_destroy_event_queue(event_queue);
}

bool MenuContainer::run(ALLEGRO_DISPLAY *display)
{
    current_menu->run(display, event_queue);
    if (action() == POSTMENUACTION::OPEN_LOBBY)
    {
        current_menu = std::unique_ptr<Menu>(new Lobbymenu(display, *this));
        planned_action = POSTMENUACTION::NOACTION;
    }
    else if (action() == POSTMENUACTION::OPEN_MAINMENU)
    {
        current_menu = std::unique_ptr<Mainmenu>(new Mainmenu(display, *this));
        planned_action = POSTMENUACTION::NOACTION;
    }
    else if (action() == POSTMENUACTION::OPEN_OPTIONS)
    {
        current_menu = std::unique_ptr<Optionsmenu>(new Optionsmenu(display, *this));
        planned_action = POSTMENUACTION::NOACTION;
    }

    if (finished)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
