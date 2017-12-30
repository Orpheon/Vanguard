#pragma once

#include "visuals/menu.h"
#include "visuals/menuanimation.h"
#include "spriteloader.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"

#include <asio.hpp>


class Lobbymenu : public Menu
{
    public:
        Lobbymenu(ALLEGRO_DISPLAY *display, MenuContainer &owner_);
        virtual ~Lobbymenu() = default;
        void run(ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue) override;
        void refreshservers();
        void quit();

        void connectionhandler(const asio::error_code &error);
        void readhandler(const asio::error_code &error);

        MenuLoopAnimation background;
        Spriteloader spriteloader;

        std::vector<ServerData> servers;

        asio::io_service io_service;
        asio::ip::tcp::socket lobbysocket;
        asio::ip::tcp::endpoint lobbyaddress;
    protected:
    private:
        int N_SERVERS_TO_DISPLAY = 15;
        double REFRESH_PERIOD = 50;
        double MIN_REFRESH_PERIOD = 5;

        ALLEGRO_FONT *serverfont;
        int scrolloffset = 0;
        int selection = -1;
        // Can't use Timer object because of hardcoded Gamestate argument in trigger function
        double refreshtimer;
        bool connected;
        bool attempted_connection;

        WriteBuffer lobby_query;

        uint32_t async_nservers;
};
