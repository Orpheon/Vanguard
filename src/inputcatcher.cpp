#include <allegro5/allegro.h>
#include <cstdio>

#include "inputcatcher.h"
#include "engine.h"
#include "renderer.h"

InputCatcher::InputCatcher(ALLEGRO_DISPLAY *display)
{
    // Initialize keyboard (TODO: and mouse) modules
    if (!al_install_keyboard())
    {
        fprintf(stderr, "Fatal Error: Could not initialize keyboard module!");
        throw -1;
    }

    // Create an event queue, and error if it fails
    event_queue = al_create_event_queue();
    if (!event_queue)
    {
        fprintf(stderr, "Fatal Error: Could not create event queue!");
        throw -1;
    }

    // Connect the window, keyboard and mouse events to this event queue
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    // TODO: Mouse
}

InputCatcher::~InputCatcher()
{
    //dtor
}

void InputCatcher::run(Engine *engine, Renderer *renderer)
{
    ALLEGRO_EVENT event;
    // Catch all events that have stacked up this frame. al_get_next_event() returns false when event_queue is empty, and contents of event are undefined
    while (al_get_next_event(event_queue, &event))
    {
        switch (event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                // Deliberate closing, not an error
                throw 0;

            case ALLEGRO_EVENT_KEY_DOWN:
                switch (event.keyboard.keycode)
                {
                    case ALLEGRO_KEY_W:
                        printf("\nW");
                        break;
                    case ALLEGRO_KEY_S:
                        printf("\nS");
                        break;
                    case ALLEGRO_KEY_A:
                        printf("\nA");
                        break;
                    case ALLEGRO_KEY_D:
                        printf("\nD");
                        break;
                }
                fflush(stdout);
        }
    }
    // TODO: The Inputcatcher should probably not be directly modifying Renderer. I'm not sure at this point.
    // Also, this camera moving is so far frame independent. This is bad, I think all of this should go to the engine somehow.
    ALLEGRO_KEYBOARD_STATE keystate;
    al_get_keyboard_state(&keystate);
    if (al_key_down(&keystate, ALLEGRO_KEY_W))
    {
        renderer->cam_y -= 10;
    }
    if (al_key_down(&keystate, ALLEGRO_KEY_S))
    {
        renderer->cam_y += 10;
    }
    if (al_key_down(&keystate, ALLEGRO_KEY_A))
    {
        renderer->cam_x -= 10;
    }
    if (al_key_down(&keystate, ALLEGRO_KEY_D))
    {
        renderer->cam_x += 10;
    }
}
