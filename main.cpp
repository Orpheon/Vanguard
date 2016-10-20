#include <cstdio>
#include <string>
#include <memory>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <enet/enet.h>

#include "inputcatcher.h"
#include "engine.h"
#include "renderer.h"
#include "datastructures.h"
#include "global_constants.h"
#include "mainmenu.h"

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
    al_set_new_display_flags(ALLEGRO_OPENGL);
    display = al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT);
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
    ALLEGRO_FONT *font = al_load_font("gg2bold.ttf", 12, ALLEGRO_TTF_MONOCHROME);
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
    double lasttimeupdated = al_get_time();
//    bool run = true;
//    while (run)
//    {
//        if (al_get_time() - lasttimeupdated >= MENU_TIMESTEP)
//        {
//            run = mainmenu->run(display, &gametype);
//            lasttimeupdated = al_get_time();
//    }
//        }
//    delete mainmenu;

    std::unique_ptr<Engine> engine;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<InputCatcher> inputcatcher;
    std::unique_ptr<Gamestate> renderingstate;

    try
    {
        // Initialize everything
        // The various allegro initializations can throw errors
        engine = std::unique_ptr<Engine>(new Engine(isserver));
        renderer = std::unique_ptr<Renderer>(new Renderer(font));
        inputcatcher = std::unique_ptr<InputCatcher>(new InputCatcher(display));
        renderingstate = std::unique_ptr<Gamestate>(new Gamestate(engine.get()));
    }
    catch (int e)
    {
        if (e == -1)
        {
            fprintf(stderr, "\nAllegro initialization failed.");
        }
        else
        {
            fprintf(stderr, "\nUNKNOWN ERROR HAPPENED");
        }
        return -1;
    }

    engine->loadmap("lijiang");
    // FIXME: Hack to make sure the oldstate is properly initialized
    engine->update(0);
    EntityPtr myself = engine->currentstate->addplayer();

    lasttimeupdated = al_get_time();
    while (true)
    {
        try
        {
            while (al_get_time() - lasttimeupdated >= ENGINE_TIMESTEP)
            {
                inputcatcher->run(myself, engine.get(), renderer.get());
                engine->update(ENGINE_TIMESTEP);

                lasttimeupdated += ENGINE_TIMESTEP;
            }
            renderingstate->interpolate(engine->oldstate.get(), engine->currentstate.get(), (al_get_time()-lasttimeupdated)/ENGINE_TIMESTEP);
            renderer->render(display, renderingstate.get(), myself);
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
    al_destroy_display(display);
    enet_deinitialize();
    return 0;
}
