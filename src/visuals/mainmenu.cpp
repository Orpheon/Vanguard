#include "visuals/mainmenu.h"
#include "global.h"
#include "global_constants.h"

#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"

#define LEFT_MOUSE_BUTTON 1
#define RIGHT_MOUSE_BUTTON 2

Mainmenu::Mainmenu(ALLEGRO_DISPLAY *display, MenuContainer &owner_) : Menu(display, owner_), spriteloader(false),
                                                                      istypingIP(false), ipfont(nullptr)
{
    background.init("ui/Menu/background/");

    ALLEGRO_FONT *normal_button_font = al_load_font("Vanguard Title Font.ttf", 30, ALLEGRO_TTF_MONOCHROME);
    ALLEGRO_FONT *hovered_button_font = al_load_font("Vanguard Title Font.ttf", 40, ALLEGRO_TTF_MONOCHROME);
    ipfont = al_load_font("Vanguard Text Font.ttf", 10, ALLEGRO_TTF_MONOCHROME);

    int WINDOW_WIDTH = al_get_display_width(display);
    int WINDOW_HEIGHT = al_get_display_height(display);
    int initial_x = WINDOW_WIDTH*0.1;
    int initial_height = WINDOW_HEIGHT*0.4;
    double line_spacing = al_get_font_line_height(hovered_button_font)*1.2;
    int counter = 0;

    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("JOIN SERVER", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         std::bind(&Mainmenu::joinlobby, this),
                                                                         normal_button_font, hovered_button_font)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("HOST SERVER", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         std::bind(&Mainmenu::hostserver, this),
                                                                         normal_button_font, hovered_button_font)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("CONNECT MANUALLY", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         std::bind(&Mainmenu::connectmanually, this),
                                                                         normal_button_font, hovered_button_font)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("OPTIONS", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         nullptr, normal_button_font,
                                                                         hovered_button_font)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("COMMUNITY", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         nullptr, normal_button_font,
                                                                         hovered_button_font)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("CREDITS", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         nullptr, normal_button_font,
                                                                         hovered_button_font)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("QUIT", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         std::bind(&Mainmenu::quit, this),
                                                                         normal_button_font, hovered_button_font)));
}

void Mainmenu::run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue)
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

        if (istypingIP)
        {
            if (event.type == ALLEGRO_EVENT_KEY_CHAR)
            {
                // Ignore things that are typed outside focus
                if (event.keyboard.display == display)
                {
                    // I'm not going to even try messing with unicode here

                    if (event.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL)
                    {
                        if (event.keyboard.keycode == ALLEGRO_KEY_V)
                        {
                            ipstring += al_get_clipboard_text(display);
                        }
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_INSERT)
                    {
                        ipstring += al_get_clipboard_text(display);
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
                    {
                        if (ipstring.length() > 0)
                        {
                            ipstring.pop_back();
                        }
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    {
                        ipstring = "";
                        istypingIP = false;
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
                    {
                        owner.serverip = ipstring;
                        owner.planned_action = JOIN_SERVER;
                        owner.exitmenus();
                    }
                    else if (event.keyboard.unichar >= 32 and event.keyboard.unichar < 127)
                    {
                        ipstring += static_cast<char>(event.keyboard.unichar);
                    }
                }
            }
        }
        else
        {
            switch (event.type)
            {
                case ALLEGRO_EVENT_KEY_CHAR:
                    switch (event.keyboard.keycode)
                    {
                        case ALLEGRO_KEY_W:
//                        pressed_keys.JUMP = true;
                            break;
                        case ALLEGRO_KEY_S:
//                        pressed_keys.CROUCH = true;
                            break;

                        case ALLEGRO_KEY_ESCAPE:
                            // Exit menu
                            quit();
                    }

                case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                    if (event.mouse.button == LEFT_MOUSE_BUTTON)
                    {
                        for (auto& button : buttons)
                        {
                            if (button->ontop(event.mouse.x, event.mouse.y))
                            {
                                button->onclick();
                            }
                        }
                    }
            }
        }
    }

    background.update(MENU_TIMESTEP);

    ALLEGRO_MOUSE_STATE mousestate;
    al_get_mouse_state(&mousestate);

    // use mousestate.x or .y for position of mouse on screen

    // Draw
    al_draw_bitmap(spriteloader.requestsprite(background.getframepath()), 0, 0, 0);
    for (auto& button : buttons)
    {
        button->render(display, mousestate.x, mousestate.y);
    }

    if (istypingIP)
    {
        int WINDOW_WIDTH = al_get_display_width(display);
        int WINDOW_HEIGHT = al_get_display_height(display);
        int textheight = al_get_font_line_height(ipfont);

        al_draw_text(ipfont, al_map_rgb(255, 255, 255), WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0 - textheight*2,
                     ALLEGRO_ALIGN_CENTER, "Please enter the server's ip and confirm with enter:");

        al_draw_text(ipfont, al_map_rgb(255, 255, 255), WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0, ALLEGRO_ALIGN_CENTER,
                     ipstring.c_str());
    }

    al_flip_display();
}

void Mainmenu::hostserver()
{
    owner.planned_action = POSTMENUACTION::HOST_SERVER;
    owner.exitmenus();
}

void Mainmenu::joinlobby()
{
    owner.planned_action = POSTMENUACTION::OPEN_LOBBY;
}

void Mainmenu::connectmanually()
{
    istypingIP = true;
}

void Mainmenu::quit()
{
    owner.planned_action = POSTMENUACTION::QUIT;
    owner.exitmenus();
}