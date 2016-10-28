#include <allegro5/allegro.h>
#include <cstdio>
#include <fstream>

#include "inputcatcher.h"
#include "datastructures.h"
#include "renderer.h"
#include "ingameelements/character.h"
#include "global_constants.h"

#define LEFT_MOUSE_BUTTON 1
#define RIGHT_MOUSE_BUTTON 2

InputCatcher::InputCatcher(ALLEGRO_DISPLAY *display)
{
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
//    al_register_event_source(event_queue, al_get_mouse_event_source(display));

    std::ifstream configfile("config.json");
    config << configfile;
    configfile.close();
}

InputCatcher::~InputCatcher()
{
    al_destroy_event_queue(event_queue);
}

void InputCatcher::run(ALLEGRO_DISPLAY *display, InputContainer *pressed_keys, InputContainer *held_keys, double *mouse_x, double *mouse_y)
{
    pressed_keys->reset();
    held_keys->reset();

    ALLEGRO_EVENT event;
    // Catch all events that have stacked up this frame. al_get_next_event() returns false when event_queue is empty, and contents of event are undefined
    while (al_get_next_event(event_queue, &event))
    {
        switch (event.type)
        {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                // Deliberate closing, not an error
                throw 0;

            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                al_acknowledge_resize(display);
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                //Debug: print the keycode number and name of the key we press
//                printf("\n%i\t%s", event.keyboard.keycode, al_keycode_to_name(event.keyboard.keycode));

                if (event.keyboard.keycode == config["jump"] or event.keyboard.keycode == config["jump_alt1"] or event.keyboard.keycode == config["jump_alt2"])
                {
                    pressed_keys->JUMP = true;
                }
                if (event.keyboard.keycode == config["crouch"] or event.keyboard.keycode == config["crouch_alt1"] or event.keyboard.keycode == config["crouch_alt2"])
                {
                    pressed_keys->CROUCH = true;
                }
                if (event.keyboard.keycode == config["left"] or event.keyboard.keycode == config["left_alt1"] or event.keyboard.keycode == config["left_alt2"])
                {
                    pressed_keys->LEFT = true;
                }
                if (event.keyboard.keycode == config["right"] or event.keyboard.keycode == config["right_alt1"] or event.keyboard.keycode == config["right_alt2"])
                {
                    pressed_keys->RIGHT = true;
                }
                if (event.keyboard.keycode == config["right"] or event.keyboard.keycode == config["right_alt1"] or event.keyboard.keycode == config["right_alt2"])
                {
                    pressed_keys->RIGHT = true;
                }
                if (event.keyboard.keycode == config["ability1"] or event.keyboard.keycode == config["ability1_alt1"] or event.keyboard.keycode == config["ability1_alt2"])
                {
                    pressed_keys->ABILITY_1 = true;
                }
                if (event.keyboard.keycode == config["ability2"] or event.keyboard.keycode == config["ability2_alt1"] or event.keyboard.keycode == config["ability2_alt2"])
                {
                    pressed_keys->ABILITY_2 = true;
                }
                if (event.keyboard.keycode == config["ultimate"] or event.keyboard.keycode == config["ultimate_alt1"] or event.keyboard.keycode == config["ultimate_alt2"])
                {
                    pressed_keys->ULTIMATE = true;
                }
                if (event.keyboard.keycode == config["reload"] or event.keyboard.keycode == config["reload_alt1"] or event.keyboard.keycode == config["reload_alt2"])
                {
                    pressed_keys->RELOAD = true;
                }

                switch (event.keyboard.keycode)
                {
                    case ALLEGRO_KEY_ESCAPE:
                        // Exit game
                        throw 0;
                }

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                switch (event.mouse.button)
                {
                    case LEFT_MOUSE_BUTTON:
                        pressed_keys->PRIMARY_FIRE = true;
                        break;
                    case RIGHT_MOUSE_BUTTON:
                        pressed_keys->SECONDARY_FIRE = true;
                        break;
                }
        }
    }

    ALLEGRO_KEYBOARD_STATE keystate;
    al_get_keyboard_state(&keystate);
    if (al_key_down(&keystate, config["jump"]) or al_key_down(&keystate, config["jump_alt1"]) or al_key_down(&keystate, config["jump_alt2"]))
    {
        held_keys->JUMP = true;
    }
    if (al_key_down(&keystate, config["crouch"]) or al_key_down(&keystate, config["crouch_alt1"]) or al_key_down(&keystate, config["crouch_alt2"]))
    {
        held_keys->CROUCH = true;
    }
    if (al_key_down(&keystate, config["left"]) or al_key_down(&keystate, config["left_alt1"]) or al_key_down(&keystate, config["left_alt2"]))
    {
        held_keys->LEFT = true;
    }
    if (al_key_down(&keystate, config["right"]) or al_key_down(&keystate, config["right_alt1"]) or al_key_down(&keystate, config["right_alt2"]))
    {
        held_keys->RIGHT = true;
    }
    if (al_key_down(&keystate, config["ability1"]) or al_key_down(&keystate, config["ability1_alt1"]) or al_key_down(&keystate, config["ability1_alt2"]))
    {
        held_keys->ABILITY_1 = true;
    }
    if (al_key_down(&keystate, config["ability2"]) or al_key_down(&keystate, config["ability2_alt1"]) or al_key_down(&keystate, config["ability2_alt2"]))
    {
        held_keys->ABILITY_2 = true;
    }
    if (al_key_down(&keystate, config["ultimate"]) or al_key_down(&keystate, config["ultimate_alt1"]) or al_key_down(&keystate, config["ultimate_alt2"]))
    {
        held_keys->ULTIMATE = true;
    }
    if (al_key_down(&keystate, config["reload"]) or al_key_down(&keystate, config["reload_alt1"]) or al_key_down(&keystate, config["reload_alt2"]))
    {
        held_keys->RELOAD = true;
    }

    ALLEGRO_MOUSE_STATE mousestate;
    al_get_mouse_state(&mousestate);
    // FIXME: I have no idea if these constants are correct, allegro docs don't mention the specifics, just that it starts with 1.
    if (mousestate.buttons & LEFT_MOUSE_BUTTON)
    {
        held_keys->PRIMARY_FIRE = true;
    }
    if (mousestate.buttons & RIGHT_MOUSE_BUTTON)
    {
        held_keys->SECONDARY_FIRE = true;
    }

    *mouse_x = mousestate.x;
    *mouse_y = mousestate.y;
}
