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

    // Initialize primitives for drawing
    if (!al_init_primitives_addon())
    {
        fprintf(stderr, "Fatal Error: Could not initialize primitives module!");
        throw -1;
    }

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

    // Initialize networking system
    if (enet_initialize())
    {
        fprintf(stderr, "Fatal Error: Could not initialize enet!");
        throw -1;
    }

    // Create a display
    ALLEGRO_DISPLAY *display;
    al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_REQUIRE);
    al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_RESIZABLE);
    display = al_create_display(1280, 720);
    if(!display)
    {
        // FIXME: Make the error argument mean anything?
        fprintf(stderr, "Fatal Error: Could not create display\n");
        throw -1;
    }

    //load font
    //gg2 font as placeholder for now i guess
    al_init_font_addon();
    al_init_ttf_addon();
    ALLEGRO_FONT *font = al_load_font("Vanguard Main Font.ttf", 12, ALLEGRO_TTF_MONOCHROME);
    if (!font)
    {
      fprintf(stderr, "Could not load 'gg2bold.ttf'.\n");
      throw -1;
    }

//    MainMenu *mainmenu = new MainMenu(display);
    bool isserver;
    if (argc >= 2)
    {
        // If there are any arguments
        isserver = false;
    }
    else
    {
        isserver = true;
    }
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
    Renderer renderer;
    InputCatcher inputcatcher(display);
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

    EntityPtr myself(0);
    if (isserver)
    {
        myself = engine.currentstate->addplayer();
        engine.currentstate->get<Player>(myself)->spawntimer.active = true;
    }
    else
    {
        ClientNetworker *n = reinterpret_cast<ClientNetworker*>(networker.get());
        while (not n->isconnected())
        {
            n->receive(engine.currentstate.get());
        }
        myself = engine.currentstate->playerlist[engine.currentstate->playerlist.size()-1];
    }

    double enginetime = al_get_time();
    double networkertime = al_get_time();
    while (true)
    {
        try
        {
            while (al_get_time() - enginetime >= ENGINE_TIMESTEP)
            {
                networker->receive(engine.currentstate.get());
                inputcatcher.run(display, engine.currentstate.get(), networker.get(), &renderer, myself);
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
            renderingstate.interpolate(engine.oldstate.get(), engine.currentstate.get(), (al_get_time()-enginetime)/ENGINE_TIMESTEP);
            renderer.render(display, &renderingstate, myself, networker.get());
        }
        catch (int e)
        {
            if (e != 0)
            {
                fprintf(stderr, "\nError during regular loop.");
                fprintf(stderr, "\nExiting..");
            }
            al_destroy_display(display);
            return 0;
        }
    }
    al_shutdown_font_addon();
    al_shutdown_ttf_addon();
    al_destroy_display(display);
    enet_deinitialize();
    return 0;
}
