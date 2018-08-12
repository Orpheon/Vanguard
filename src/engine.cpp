#include <memory>

#include "engine.h"
#include "entity.h"
#include "gamestate.h"
#include "global_constants.h"

Engine::Engine(bool isserver_) : currentstate(new Gamestate(*this)), oldstate(new Gamestate(*this)), maskloader(),
                                 isserver(isserver_), maprotationindex(0)
{
    ;// constructor
}

Engine::~Engine()
{
    ;// destructor
}

void Engine::loadrotation(std::vector<std::string> &mapnames)
{
    maprotation = mapnames;
    // TODO: Check for existence of all maps in the rotation here
}

void Engine::nextmap()
{
    std::string name = maprotation[maprotationindex];
    maprotationindex = (maprotationindex+1) % maprotation.size();
    if (isserver)
    {
        sendbuffer.write<uint8_t>(MAPSTART);
        sendbuffer.write<uint32_t>(name.length());
        sendbuffer.writestring(name);
    }
    currentstate->loadmap(name);
}

void Engine::update(double frametime)
{
    oldstate = currentstate->clone();
    currentstate->update(frametime);
}
