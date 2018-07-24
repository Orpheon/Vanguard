#include <cstdio>
#include <string>
#include <memory>
#include <fstream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <SFML/Graphics/RenderWindow.hpp>
#define boolean enet_boolean
#include <enet/enet.h>

#undef boolean

#include "inputcatcher.h"
#include "engine.h"
#include "renderer.h"
#include "datastructures.h"
#include "global_constants.h"
#include "visuals/mainmenu.h"
#include "networking/servernetworker.h"
#include "networking/clientnetworker.h"
#include "global.h"
#include "configloader.h"

long int getmillisec();

int main(int argc, char **argv)
{
    try
    {
        // Initialize logging
        std::unique_ptr<PrintLogger> default_logger(new PrintLogger());
        Global::provide_logging(default_logger.get());

        // Load the settings config
        ConfigLoader settings_configloader;
        nlohmann::json settings = settings_configloader.open("settings.json");
        Global::provide_settings(&settings);

        // Initialize networking system
        if (enet_initialize())
        {
            Global::logging().panic(__FILE__, __LINE__, "Enet initialization failed");
        }

        int window_width, window_height, window_type;

        window_width = Global::settings().at("Display resolution").at(0);
        window_height = Global::settings().at("Display resolution").at(1);
        // TODO: Add window settings in config

        sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Vanguard");
        window.setKeyRepeatEnabled(false);

        std::unique_ptr<MenuContainer> menus = std::unique_ptr<MenuContainer>(new MenuContainer(window));
        double lasttimeupdated = al_get_time();
        int not_finished = 1;
        while (not_finished)
        {
            if (al_get_time() - lasttimeupdated >= MENU_TIMESTEP)
            {
                not_finished = menus->run(window);
                lasttimeupdated = al_get_time();
                if (not_finished == -1)
                {
                    // Quit
                    return 0;
                }
            }
        }

        bool isserver = true;
        std::string serverip;
        int serverport = 3224;

        // Server, client, or quit?
        if (menus->action() == POSTMENUACTION::QUIT)
        {
            // Exit peacefully
            return 0;
        }
        else if (menus->action() == POSTMENUACTION::HOST_SERVER)
        {
            isserver = true;
        }
        else if (menus->action() == POSTMENUACTION::JOIN_SERVER)
        {
            isserver = false;
            serverip = menus->serverip;
            serverport = menus->serverport;
        }
        else
        {
            Global::logging().panic(__FILE__, __LINE__, "Unknown post menu status %i", menus->action());
        }

        // Clean up
        menus.reset();

        Engine engine(isserver);
        InputCatcher inputcatcher;
        Gamestate renderingstate(engine);
        Renderer renderer;

        std::unique_ptr<Networker> networker;
        if (isserver)
        {
            networker = std::unique_ptr<Networker>(new ServerNetworker(engine.sendbuffer));
            std::ifstream maprotationfile("maprotation.json");
            if (not maprotationfile.good())
            {
                Global::logging().panic(__FILE__, __LINE__, "maprotation.json was not found.");
            }
            nlohmann::json maprotationdata;
            maprotationdata << maprotationfile;
            maprotationfile.close();
            std::vector<std::string> maprotation = maprotationdata["maps"];
            engine.loadrotation(maprotation);
            engine.nextmap();
            // FIXME: Hack to make sure the oldstate is properly initialized
            engine.update(0);
        }
        else
        {
            networker = std::unique_ptr<Networker>(new ClientNetworker(engine.sendbuffer, serverip, serverport));
        }

        EntityPtr myself(0);
        if (isserver)
        {
            myself = engine.currentstate->addplayer();
            Player &p = engine.currentstate->get<Player&>(myself);
            p.name = Global::settings()["Player name"];
            p.spawntimer.active = true;
        }
        else
        {
            ClientNetworker &n = reinterpret_cast<ClientNetworker&>(*networker);
            while (not n.isconnected())
            {
                n.receive(*(engine.currentstate));
            }
            myself = engine.currentstate->playerlist.at(engine.currentstate->playerlist.size()-1);
        }

        double enginetime = al_get_time();
        double networkertime = al_get_time();
        while (true)
        {
            while (al_get_time() - enginetime >= ENGINE_TIMESTEP)
            {
                networker->receive(*(engine.currentstate));
                inputcatcher.run(window, *(engine.currentstate), *networker, renderer, myself);
                engine.update(ENGINE_TIMESTEP);
                networker->sendeventdata(*(engine.currentstate));

                enginetime += ENGINE_TIMESTEP;
            }
            if (isserver)
            {
                if (al_get_time() - networkertime >= NETWORKING_TIMESTEP)
                {
                    ServerNetworker &n = reinterpret_cast<ServerNetworker&>(*networker);
                    n.sendframedata(*(engine.currentstate));

                    networkertime = al_get_time();
                }
            }
            renderingstate.interpolate(*(engine.oldstate), *(engine.currentstate), (al_get_time()-enginetime)/ENGINE_TIMESTEP);
            renderer.render(window, renderingstate, myself, *networker);
        }
    }
    catch (...)
    {
        return 0;
    }
}