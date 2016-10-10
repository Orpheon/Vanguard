#include <allegro5/allegro.h>
#include <cstdio>

#include "inputcatcher.h"
#include "datastructures.h"
#include "renderer.h"
#include "character.h"
#include "global_constants.h"

#define LEFT_MOUSE_BUTTON 1
#define RIGHT_MOUSE_BUTTON 2

InputCatcher::InputCatcher(ALLEGRO_DISPLAY *display)
{
    // Initialize keyboard modules
    if (!al_install_keyboard())
    {
        fprintf(stderr, "Fatal Error: Could not initialize keyboard module!");
        throw -1;
    }

    // Initialize mouse
    if (!al_install_mouse())
    {
        fprintf(stderr, "Fatal Error: Could not initialize mouse module!");
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

void InputCatcher::run(EntityPtr myself, Engine *engine, Renderer *renderer)
{
    // FIXME: Debugtool
    bool spawnplayer = false;

    INPUT_CONTAINER pressed_keys = {};
    INPUT_CONTAINER held_keys = {};

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
                        pressed_keys.JUMP = true;
                        break;
                    case ALLEGRO_KEY_S:
                        pressed_keys.CROUCH = true;
                        break;
                    case ALLEGRO_KEY_A:
                        pressed_keys.LEFT = true;
                        break;
                    case ALLEGRO_KEY_D:
                        pressed_keys.RIGHT = true;
                        break;
                    case ALLEGRO_KEY_LSHIFT:
                        pressed_keys.ABILITY_1 = true;
                        break;
                    case ALLEGRO_KEY_E:
                        pressed_keys.ABILITY_2 = true;
                        break;
                    case ALLEGRO_KEY_Q:
                        pressed_keys.ULTIMATE = true;
                        break;

                    // FIXME: Debugtool
                    case ALLEGRO_KEY_R:
                        spawnplayer = true;
                        break;

                    case ALLEGRO_KEY_ESCAPE:
                        // Exit game
                        throw 0;
                }

            // FIXME: BROKEN DOES NOT WORK
            // event.mouse.button seems to always be 0
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                switch (event.mouse.button)
                {
                    case LEFT_MOUSE_BUTTON:
                        pressed_keys.PRIMARY_FIRE = true;
                        break;
                    case RIGHT_MOUSE_BUTTON:
                        pressed_keys.SECONDARY_FIRE = true;
                        break;
                }
        }
    }

    ALLEGRO_KEYBOARD_STATE keystate;
    al_get_keyboard_state(&keystate);
    if (al_key_down(&keystate, ALLEGRO_KEY_W))
    {
        held_keys.JUMP = true;
    }
    if (al_key_down(&keystate, ALLEGRO_KEY_S))
    {
        held_keys.CROUCH = true;
    }
    if (al_key_down(&keystate, ALLEGRO_KEY_A))
    {
        held_keys.LEFT = true;
    }
    if (al_key_down(&keystate, ALLEGRO_KEY_D))
    {
        held_keys.RIGHT = true;
    }
    if (al_key_down(&keystate, ALLEGRO_KEY_LSHIFT))
    {
        held_keys.ABILITY_1 = true;
    }
    if (al_key_down(&keystate, ALLEGRO_KEY_E))
    {
        held_keys.ABILITY_2 = true;
    }
    if (al_key_down(&keystate, ALLEGRO_KEY_Q))
    {
        held_keys.ULTIMATE = true;
    }

    ALLEGRO_MOUSE_STATE mousestate;
    al_get_mouse_state(&mousestate);
    // FIXME: I have no idea if these constants are correct, allegro docs don't mention the specifics, just that it starts with 1.
    if (mousestate.buttons & LEFT_MOUSE_BUTTON)
    {
        held_keys.PRIMARY_FIRE = true;
    }
    if (mousestate.buttons & RIGHT_MOUSE_BUTTON)
    {
        held_keys.SECONDARY_FIRE = true;
    }


    // Check if the current player has a character to run around with or is in spectate mode
    Character *c = engine->currentstate->get<Player>(myself)->getcharacter(engine->currentstate.get());
    if (c != 0)
    {
        c->setinput(engine->currentstate.get(), pressed_keys, held_keys, mousestate.x+renderer->cam_x-c->x, mousestate.y+renderer->cam_y-c->y);
    }
    else
    {
        // DEBUGTOOL
        if (spawnplayer)
        {
            engine->currentstate->get<Player>(myself)->spawn(engine->currentstate.get(), renderer->cam_x+mousestate.x, renderer->cam_y+mousestate.y);
        }
        if (held_keys.LEFT)
        {
            renderer->cam_x -= 10;
        }
        if (held_keys.RIGHT)
        {
            renderer->cam_x += 10;
        }
        if (held_keys.JUMP)
        {
            renderer->cam_y -= 10;
        }
        if (held_keys.CROUCH)
        {
            renderer->cam_y += 10;
        }
    }
}
