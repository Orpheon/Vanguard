
#include "visuals/lobbymenu.h"
#include "global_constants.h"
#include "networking/uuid.h"

#include "allegro5/allegro_primitives.h"

#include <cstdint>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

Lobbymenu::Lobbymenu(sf::RenderWindow &window, MenuContainer &owner_) : Menu(window, owner_), lobbysocket(io_service)
{
    background.init("ui/menus/mainmenu/");
    if (not serverfont.loadFromFile("Vanguard Text Font.ttf"))
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not find \"Vanguard Text Font.ttf\".");
    }

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

void Lobbymenu::run(sf::RenderWindow &window)
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
    sf::Vector2u windowsize = window.getSize();
    int initial_x = windowsize.x * 0.1;
    int linewidth = windowsize.x - initial_x*2.0;
    int spacing = 10;
    int fontsize = 15;
    // No way fontsize can be used like that, but idk how. Ideally these would be width of "9999" and "Players"
    int ping_x = initial_x + linewidth - fontsize*4 - spacing;
    int playercount_x = ping_x - spacing - fontsize*7;
    int map_x = (playercount_x - spacing - initial_x) * 2/3.0 + initial_x;

    int initial_y = windowsize.y * 0.2;
    int lineheight = 30;
    int linespacing = 6;
    int servers_y = initial_y + 3*(lineheight + linespacing);
    int text_height = initial_y + (lineheight - fontsize)/2.0;

    // Get input
    sf::Vector2i mousepos = sf::Mouse::getPosition(window);
    if (mousepos.x > initial_x and mousepos.x < initial_x + linewidth)
    {
        int total_height = lineheight * N_SERVERS_TO_DISPLAY + linespacing * (N_SERVERS_TO_DISPLAY-1);
        double unit = (total_height+linespacing) / N_SERVERS_TO_DISPLAY;

        if (mousepos.y > servers_y and mousepos.y < servers_y + total_height)
        {
            selection = static_cast<int>((mousepos.y - servers_y) / unit);
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

    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                quit();
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                {
                    // Go back to main menu
                    owner.planned_action = POSTMENUACTION::OPEN_MAINMENU;
                }
                else if (event.key.code == sf::Keyboard::F5)
                {
                    if (refreshtimer < REFRESH_PERIOD - MIN_REFRESH_PERIOD)
                    {
                        refreshtimer = 0;
                    }
                }
                break;

            case sf::Event::MouseWheelScrollEvent:
                int n_servers = static_cast<signed int>(servers.size());
                int delta = static_cast<int>(event.mouseWheelScroll.delta);
                if (scrolloffset - delta > n_servers - N_SERVERS_TO_DISPLAY)
                {
                    scrolloffset = n_servers - N_SERVERS_TO_DISPLAY;
                }
                else
                {
                    scrolloffset -= delta;
                }
                if (scrolloffset < 0)
                {
                    scrolloffset = 0;
                }
                break;

            case sf::Event::MouseButtonReleased:
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

//    background.update(MENU_TIMESTEP);
//
//    sf::Vector2i mousepos = sf::Mouse::getPosition(window);
//    sf::Vector2u windowsize = window.getSize();
//
//    // Draw
//    int WINDOW_WIDTH = windowsize.x;
//    int WINDOW_HEIGHT = windowsize.y;
//    sf::Sprite bgsprite;
//    spriteloader.loadsprite(background.getframepath(), bgsprite);
//
//    window.draw(bgsprite);
//
//    for (auto& button : buttons)
//    {
//        button->render(window, mousepos.x, mousepos.y);
//    }
//
//    if (istypingIP)
//    {
//        sf::Text iptext;
//        iptext.setCharacterSize(10);
//        iptext.setString("Please enter the server's ip and confirm with enter:");
//        iptext.setFont(textfont);
//        iptext.setOrigin(0, iptext.getLocalBounds().height * 2.0);
//        iptext.setPosition(WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0);
//        window.draw(iptext);
//
//        iptext.setString(ipstring);
//        iptext.setOrigin(0, 0);
//        window.draw(iptext);
//    }
//
//    window.display();


    background.update(MENU_TIMESTEP);

    // Draw
    int WINDOW_WIDTH = windowsize.x;
    int WINDOW_HEIGHT = windowsize.y;
    sf::Sprite bgsprite;
    spriteloader.loadsprite(background.getframepath(), bgsprite);

    sf::RectangleShape backdrop(sf::Vector2f(linewidth, lineheight));
    backdrop.setPosition(initial_x, initial_y);
    backdrop.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(backdrop);

    sf::Text text;
    text.setCharacterSize(fontsize);
    text.setFont(serverfont);

    text.setPosition(initial_x + spacing, text_height);
    text.setString("Name");
    window.draw(text);
    text.setPosition(ping_x + spacing, text_height);
    text.setString("Ping");
    window.draw(text);
    text.setPosition(playercount_x, text_height);
    text.setString("Players");
    window.draw(text);
    text.setPosition(map_x + spacing, text_height);
    text.setString("Map Name");
    window.draw(text);

    text_height = servers_y + (lineheight - fontsize)/2.0;

    auto serveridx = servers.begin() + scrolloffset;
    for (int i = 0; i < N_SERVERS_TO_DISPLAY and serveridx != servers.end(); ++i)
    {
        auto& server = *serveridx;

        backdrop.setPosition(initial_x, servers_y);
        window.draw(backdrop);

        if (selection == i)
        {
            backdrop.setPosition(initial_x, servers_y);
            backdrop.setFillColor(sf::Color::Transparent);
            backdrop.setOutlineColor(sf::Color(0, 255, 255, 100));
            backdrop.setOutlineThickness(linespacing*2/3.0);
            window.draw(backdrop);
        }

        text.setPosition(initial_x + spacing, text_height);
        text.setString(server.name);
        window.draw(text);
        text.setPosition(map_x, text_height);
        text.setString(server.mapname);
        window.draw(text);
        text.setPosition(playercount_x, text_height);
        text.setString(std::to_string(server.playercount));
        window.draw(text);

        text.setPosition(ping_x, text_height);
        text.setString(std::to_string(server.ping));
        if (server.ping < 135)
        {
            text.setFillColor(sf::Color::Green);
        }
        else if (server.ping < 225)
        {
            text.setFillColor(sf::Color::Yellow);
        }
        else
        {
            text.setFillColor(sf::Color::Red);
        }
        window.draw(text);

        servers_y += lineheight + linespacing;
        text_height = servers_y + (lineheight - fontsize)/2.0;
        ++serveridx;
    }

    window.display();
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
