
#include "visuals/lobbymenu.h"
#include "global_constants.h"

#include "allegro5/allegro_primitives.h"

Lobbymenu::Lobbymenu(ALLEGRO_DISPLAY *display, MenuContainer &owner_) : Menu(display, owner_), spriteloader(false)
{
    for (int i=0; i<100; ++i)
    {
        ServerData s = {"Server"+std::to_string(i), "1.1.1.1", "lijiang", i, i*3, (i%20)*30};
        servers.push_back(s);
    }

    background.init("ui/Menu/background/");

    serverfont = al_load_font("Vanguard Text Font.ttf", 15, ALLEGRO_TTF_MONOCHROME);
}

void Lobbymenu::run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue)
{
    // Define frame
    int initial_x = al_get_display_width(display) * 0.1;
    int linewidth = al_get_display_width(display) - initial_x*2.0;
    int spacing = 10;
    int ping_x = initial_x + linewidth - al_get_text_width(serverfont, "9999") - spacing;
    int playercount_x = ping_x - spacing - al_get_text_width(serverfont, "Players");
    int map_x = (playercount_x - spacing - initial_x) * 2/3.0 + initial_x;

    int initial_y = al_get_display_height(display) * 0.2;
    int lineheight = 30;
    int linespacing = 6;
    int servers_y = initial_y + 3*(lineheight + linespacing);
    int text_height = initial_y + (lineheight - al_get_font_line_height(serverfont))/2.0;


    // Get input
    ALLEGRO_MOUSE_STATE current_mouse_state;
    al_get_mouse_state(&current_mouse_state);
    int mouse_x = current_mouse_state.x, mouse_y = current_mouse_state.y;
    if (mouse_x > initial_x and mouse_x < initial_x + linewidth)
    {
        int total_height = lineheight * N_SERVERS_TO_DISPLAY + linespacing * (N_SERVERS_TO_DISPLAY-1);
        double unit = (total_height+linespacing) / N_SERVERS_TO_DISPLAY;

        if (mouse_y > servers_y and mouse_y < servers_y + total_height)
        {
            selection = static_cast<int>((mouse_y - servers_y) / unit);
        }
        else
        {
            selection = -1;
        }
    }
    else
    {
        selection = -1;
    }

    ALLEGRO_EVENT event;
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
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
                if (event.mouse.dz != 0)
                {
                    int n_servers = static_cast<signed int>(servers.size());
                    if (scrolloffset - event.mouse.dz > n_servers - N_SERVERS_TO_DISPLAY)
                    {
                        scrolloffset = n_servers - N_SERVERS_TO_DISPLAY;
                    }
                    else
                    {
                        scrolloffset -= event.mouse.dz;
                    }

                    if (scrolloffset < 0)
                    {
                        scrolloffset = 0;
                    }
                }
                break;
        }
    }


    // Draw
    background.update(MENU_TIMESTEP);

    al_draw_bitmap(spriteloader.requestsprite(background.getframepath()), 0, 0, 0);

    al_draw_filled_rectangle(initial_x, initial_y, initial_x+linewidth, initial_y+lineheight,
                             al_map_rgba(0, 0, 0, 100));
    al_draw_text(serverfont, al_map_rgb(255, 255, 255), initial_x + spacing, text_height, ALLEGRO_ALIGN_LEFT, "Name");
    al_draw_text(serverfont, al_map_rgb(255, 255, 255), ping_x, text_height, ALLEGRO_ALIGN_LEFT, "Ping");
    al_draw_text(serverfont, al_map_rgb(255, 255, 255), playercount_x, text_height, ALLEGRO_ALIGN_LEFT, "Players");
    al_draw_text(serverfont, al_map_rgb(255, 255, 255), map_x, text_height, ALLEGRO_ALIGN_LEFT, "Map Name");

    text_height = servers_y + (lineheight - al_get_font_line_height(serverfont))/2.0;

    auto serveridx = servers.begin() + scrolloffset;
    for (int i = 0; i < N_SERVERS_TO_DISPLAY and serveridx != servers.end(); ++i)
    {
        auto& server = *serveridx;

        al_draw_filled_rectangle(initial_x, servers_y, initial_x+linewidth, servers_y+lineheight,
                                 al_map_rgba(0, 0, 0, 100));

        if (selection == i)
        {
            al_draw_rectangle(initial_x, servers_y - linespacing*1/3.0, initial_x + linewidth,
                              servers_y + lineheight + linespacing*1/3.0, al_map_rgba(0, 255, 255, 100),
                              linespacing*2/3.0);
        }

        al_draw_text(serverfont, al_map_rgb(255, 255, 255), initial_x + spacing, text_height, ALLEGRO_ALIGN_LEFT,
                     server.name.c_str());
        al_draw_text(serverfont, al_map_rgb(255, 255, 255), map_x, text_height, ALLEGRO_ALIGN_LEFT,
                     server.mapname.c_str());
        al_draw_text(serverfont, al_map_rgb(255, 255, 255), playercount_x, text_height, ALLEGRO_ALIGN_LEFT,
                     (std::to_string(server.playercount) + "/" + std::to_string(server.maxplayercount)).c_str());
        if (server.ping < 135)
        {
            al_draw_text(serverfont, al_map_rgb(0, 255, 0), ping_x, text_height, ALLEGRO_ALIGN_LEFT,
                         std::to_string(server.ping).c_str());
        }
        else if (server.ping < 225)
        {
            al_draw_text(serverfont, al_map_rgb(255, 255, 0), ping_x, text_height, ALLEGRO_ALIGN_LEFT,
                         std::to_string(server.ping).c_str());
        }
        else
        {
            al_draw_text(serverfont, al_map_rgb(255, 0, 0), ping_x, text_height, ALLEGRO_ALIGN_LEFT,
                         std::to_string(server.ping).c_str());
        }

        servers_y += lineheight + linespacing;
        text_height = servers_y + (lineheight - al_get_font_line_height(serverfont))/2.0;
        ++serveridx;
    }

    al_flip_display();
}

void Lobbymenu::quit()
{
    owner.planned_action = POSTMENUACTION::QUIT;
    owner.exitmenus();
}
