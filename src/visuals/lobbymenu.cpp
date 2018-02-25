
#include "visuals/lobbymenu.h"
#include "global_constants.h"
#include "networking/uuid.h"

#include "allegro5/allegro_primitives.h"

#include <cstdint>

Lobbymenu::Lobbymenu(ALLEGRO_DISPLAY *display, MenuContainer &owner_) : Menu(display, owner_), spriteloader(false),
                                                                        lobbysocket(io_service)
{
    background.init("ui/menus/mainmenu/");
    serverfont = al_load_font("Vanguard Text Font.ttf", 15, ALLEGRO_TTF_MONOCHROME);

    connected = false;
    attempted_connection = false;
    asio::ip::tcp::resolver ipresolver(io_service);
    asio::ip::tcp::resolver::query query(LOBBY_HOST, std::to_string(LOBBY_PORT));
    lobbyaddress = *ipresolver.resolve(query);

    refreshtimer = 0;
    async_nservers = 0;

    // See https://github.com/Medo42/Faucet-Lobby/blob/master/Protocol%20Spec.txt for reference
    xg::Guid message_type(LOBBY_MESSAGE_TYPE_LIST);
    xg::Guid lobbyid(VANGUARD_IDENTIFIER);

    // Message type for lobby
    for (auto& byte : message_type._bytes)
    {
        lobby_query.write<uint8_t>(byte, true);
    }
    // Lobby id
    for (auto& byte : lobbyid._bytes)
    {
        lobby_query.write<uint8_t>(byte, true);
    }
}

void Lobbymenu::run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue)
{
    // Networking
    io_service.reset();
    io_service.poll();

    refreshtimer -= MENU_TIMESTEP;
    if (refreshtimer <= 0)
    {
        if (connected)
        {
            refreshservers();
        }
        else if (not attempted_connection)
        {
            lobbysocket.async_connect(lobbyaddress,
                                      std::bind(&Lobbymenu::connectionhandler, this, std::placeholders::_1));
            attempted_connection = true;
        }
    }

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
                else if (event.keyboard.keycode == ALLEGRO_KEY_F5)
                {
                    if (refreshtimer < REFRESH_PERIOD - MIN_REFRESH_PERIOD)
                    {
                        refreshtimer = 0;
                    }
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

            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                if (selection != -1 and selection < static_cast<signed int>(servers.size()))
                {
                    owner.planned_action = POSTMENUACTION::JOIN_SERVER;
                    owner.serverip = servers.at(selection).ip;
                    owner.serverport = servers.at(selection).port;
                    owner.exitmenus();
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

void Lobbymenu::connectionhandler(const asio::error_code &error)
{
    if (not error)
    {
        connected = true;
    }
    else
    {
        Global::logging().print(__FILE__, __LINE__, "Error connecting to lobby: %s", error.message().c_str());
    }
}

void Lobbymenu::readhandler(const asio::error_code &error)
{
    if (error)
    {
        Global::logging().print(__FILE__, __LINE__, "Error receiving data from lobby: %s", error.message().c_str());
    }
    else
    {
        int n_servers = __builtin_bswap32(async_nservers);
        servers.clear();
        for (int i=0; i<n_servers; ++i)
        {
            ServerData new_server;

            uint32_t serverblocklength = 0;
            asio::read(lobbysocket, asio::buffer(&serverblocklength, sizeof(uint32_t)));
            serverblocklength = __builtin_bswap32(serverblocklength);

            void *data = calloc(serverblocklength, 1);
            asio::read(lobbysocket, asio::buffer(data, serverblocklength));
            ReadBuffer buffer(data, serverblocklength);
            buffer.read<uint8_t>(); // UDP / TCP
            int ipv4_port = __builtin_bswap16(buffer.read<uint16_t>());
            int p1 = buffer.read<uint8_t>();
            int p2 = buffer.read<uint8_t>();
            int p3 = buffer.read<uint8_t>();
            int p4 = buffer.read<uint8_t>();
            std::string ipv4_addr = std::to_string(p1) + "." + std::to_string(p2) + "." + std::to_string(p3) + "."
                                    + std::to_string(p4);
            // We don't support ipv6
            buffer.read<uint16_t>(); // Port number
            buffer.read<uint32_t>(); // Address 0-4
            buffer.read<uint32_t>(); // Address 5-8
            buffer.read<uint32_t>(); // Address 9-12
            buffer.read<uint32_t>(); // Address 13-16
            if (ipv4_port != 0) {
                new_server.ip = ipv4_addr;
                new_server.port = ipv4_port;
            } else {
                std::free(data);
                continue;
            }
            int n_maxplayers = __builtin_bswap16(buffer.read<uint16_t>());
            int n_players = __builtin_bswap16(buffer.read<uint16_t>());
            int n_bots = __builtin_bswap16(buffer.read<uint16_t>());
            new_server.maxplayercount = n_maxplayers;
            new_server.playercount = n_players;
            new_server.botcount = n_bots;
            // Flags currently only containing password protection, which we're ignoring for now
            buffer.read<uint16_t>();
            __builtin_bswap16(buffer.read<uint16_t>()); // length of key/value data
            while (buffer.length() > 0) {
                int keylength = buffer.read<uint8_t>();
                std::string key = buffer.readstring(keylength);
                int valuelength = __builtin_bswap16(buffer.read<uint16_t>());
                std::string value = buffer.readstring(valuelength);
                if (key == "map") {
                    new_server.mapname = value;
                } else if (key == "name") {
                    new_server.name = value;
                }
            }

            std::free(data);
            servers.push_back(new_server);
        }

        lobbysocket.close();
        connected = false;
        attempted_connection = false;
    }
}

void Lobbymenu::refreshservers()
{
    asio::write(lobbysocket, asio::buffer(lobby_query.getdata(), lobby_query.length()), asio::transfer_all());
    asio::async_read(lobbysocket, asio::buffer(&async_nservers, sizeof(uint32_t)), asio::transfer_all(),
                     std::bind(&Lobbymenu::readhandler, this, std::placeholders::_1));

    refreshtimer = REFRESH_PERIOD;
}

void Lobbymenu::quit()
{
    owner.planned_action = POSTMENUACTION::QUIT;
    owner.exitmenus();
}
