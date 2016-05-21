#include <cstdio>
#include <allegro5/allegro.h>

#include "engine.h"
#include "renderer.h"
#include "global_constants.h"

long int getmillisec();

int main(int argc, char **argv)
{
	try
	{
		Engine* engine = new Engine();
		Renderer* renderer = new Renderer();
	}
	catch (int e)
	{
		fprintf("\nInitialization failed.");
		return -1;
	}

    while (true)
    {
		engine->run();
		renderer->render();
    }
    return 0;
}
