#include <cstdio>
#include <string>
#include <memory>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#define boolean enet_boolean
#include <enet/enet.h>
#undef boolean

#include "inputcatcher.h"
#include "engine.h"
#include "renderer.h"
#include "datastructures.h"
#include "global_constants.h"
#include "mainmenu.h"
#include "networking/servernetworker.h"
#include "networking/clientnetworker.h"

long int getmillisec();

int main(int argc, char **argv)
{
    // Initialize Allegro
    if (!al_init())
    {
        fprintf(stderr, "Fatal Error: Allegro initialization failed!\n");
        return -1;
    }

    // Initialize the Allegro Image addon, used to load sprites and maps
    if (!al_init_image_addon())
    {
        fprintf(stderr, "Fatal Error: Allegro Image Addon initialization failed!\n");
        return -1;
    }

    // Initialize networking system
    if (enet_initialize())
    {
        fprintf(stderr, "Fatal Error: Could not initialize enet!");
        throw -1;
    }

//    MainMenu *mainmenu = new MainMenu(display);
    bool isserver = true;
//    double lasttimeupdated = al_get_time();
//    bool run = true;
//    while (run)
//    {
//        if (al_get_time() - lasttimeupdated >= MENU_TIMESTEP)
//        {
//            run = mainmenu->run(display, &gametype);
//            lasttimeupdated = al_get_time();
//        }
//    }
//    delete mainmenu;

    Engine engine(isserver);
    Gamestate renderingstate(&engine);

    std::unique_ptr<Networker> networker;
    if (isserver)
    {
        networker = std::unique_ptr<Networker>(new ServerNetworker());
    }
    else
    {
        networker = std::unique_ptr<Networker>(new ClientNetworker());
    }

    engine.loadmap("lijiang");
    // FIXME: Hack to make sure the oldstate is properly initialized
    engine.update(&(networker->sendbuffer), 0);

    double enginetime = al_get_time();
    double networkertime = al_get_time();
    while (true)
    {
        try
        {
            while (al_get_time() - enginetime >= ENGINE_TIMESTEP)
            {
                networker->receive(engine.currentstate.get());
                engine.update(&(networker->sendbuffer), ENGINE_TIMESTEP);
                networker->sendeventdata(engine.currentstate.get());

                enginetime += ENGINE_TIMESTEP;
            }
            if (isserver)
            {
                if (al_get_time() - networkertime >= NETWORKING_TIMESTEP)
                {
                    ServerNetworker *n = reinterpret_cast<ServerNetworker*>(networker.get());
                    n->sendframedata(engine.currentstate.get());

                    networkertime = al_get_time();
                }
            }
        }
        catch (int e)
        {
            if (e != 0)
            {
                fprintf(stderr, "\nError during regular loop.");
                fprintf(stderr, "\nExiting..");
            }
            return 0;
        }
    }
    enet_deinitialize();
    return 0;
}
