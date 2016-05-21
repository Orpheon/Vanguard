#include <cstdio>
#include <allegro5/allegro.h>

#include "engine.h"
#include "renderer.h"
#include "global_constants.h"

long int getmillisec();

int main(int argc, char **argv)
{
	Engine *engine;
	Renderer *renderer;

	try
	{
		// Initialize everything
		// The various allegro initializations can throw errors
		engine = new Engine();
		renderer = new Renderer();
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
		engine->run();
		renderer->render();
    }
    return 0;
}
