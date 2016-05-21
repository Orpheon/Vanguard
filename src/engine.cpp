#include <cstdio>
#include <sys/time.h>
#include <list>

#include "engine.h"
#include "entity.h"
#include "global_constants.h"

Engine::Engine()
{
	lasttimeupdated = getmillisec();
	millisecperframe = 1000.0 / PHYSICS_FPS;
	std::list<Entity*> gameentities;
}

Engine::~Engine()
{
    //destructor
}

void Engine::run()
{
	int timediff = getmillisec() - lasttimeupdated;
	while (timediff - millisecperframe >= 0)
	{
		// TODO Actual physics here
		lasttimeupdated += millisecperframe;
		timediff -= millisecperframe;
	}
}

long int Engine::getmillisec()
{
    static struct timeval tp;
    gettimeofday(&tp, 0);
    return ((long) tp.tv_sec) * 1000 + ((long) tp.tv_usec) / 1000;
}
