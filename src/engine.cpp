#include <cstdio>
#include <sys/time.h>
#include <string.h>
#include <memory>

#include "engine.h"
#include "entity.h"
#include "gamestate.h"
#include "global_constants.h"

Engine::Engine()
{
    lasttimeupdated = getmillisec();
    millisecperframe = 1000.0 / PHYSICS_FPS;
    currentstate = Gamestate();
}

Engine::~Engine()
{
    ;// destructor
}

void Engine::loadmap(std::string mapname)
{
    currentstate.currentmap = std::make_shared<Map>(mapname);
}

void Engine::run()
{
    int timediff = getmillisec() - lasttimeupdated;
    while (timediff - millisecperframe >= 0)
    {
        currentstate.update(millisecperframe / 1000.0);
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

PlayerPtr Engine::newplayer()
{
    return currentstate.make_player();
}
