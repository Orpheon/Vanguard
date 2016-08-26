#include <cstdio>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include "inputcatcher.h"
#include "engine.h"
#include "renderer.h"
#include "player.h"
#include "global_constants.h"

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

    InputCatcher *inputcatcher;
    Engine *engine;
    Renderer *renderer;
    Player *myself;

    try
    {
        // Initialize everything
        // The various allegro initializations can throw errors
        engine = new Engine();
        renderer = new Renderer();
        inputcatcher = new InputCatcher(renderer->display);
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

    engine->loadmap("conflict");
    myself = engine->newplayer();

    while (true)
    {
        inputcatcher->run(myself, engine, renderer);
        engine->run();
        renderer->render(engine->currentstate);
    }
    return 0;
}
