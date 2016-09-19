#include <cstdio>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <sys/time.h>

#include "inputcatcher.h"
#include "engine.h"
#include "renderer.h"
#include "datastructures.h"
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
    PlayerPtr myself = engine->newplayer();

    long int lasttimeupdated = getmillisec();
    while (true)
    {
        while (getmillisec() - lasttimeupdated >= 1000.0*ENGINE_TIMESTEP)
        {
            inputcatcher->run(myself, engine, renderer);
            engine->update(ENGINE_TIMESTEP);

            lasttimeupdated += 1000.0*ENGINE_TIMESTEP;
        }
        renderer->render(&(engine->currentstate), myself);
    }
    return 0;
}


long int getmillisec()
{
    static struct timeval tp;
    gettimeofday(&tp, 0);
    return ((long) tp.tv_sec) * 1000 + ((long) tp.tv_usec) / 1000;
}
