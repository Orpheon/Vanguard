#include <cstdio>
#include <allegro5/allegro.h>

#include "inputcatcher.h"
#include "engine.h"
#include "renderer.h"
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

    while (true)
    {
		inputcatcher->run();
		engine->run();
		renderer->render();
    }
    return 0;
}
