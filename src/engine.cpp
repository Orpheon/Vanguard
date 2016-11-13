#include <memory>

#include "engine.h"
#include "entity.h"
#include "gamestate.h"
#include "global_constants.h"

Engine::Engine(bool isserver_) : currentstate(new Gamestate(this)), oldstate(new Gamestate(this)), maskloader(true), isserver(isserver_)
{
    ;// constructor
}

Engine::~Engine()
{
    ;// destructor
}

void Engine::loadmap(std::string mapname)
{
    currentstate->currentmap = std::make_shared<Map>(currentstate.get(), mapname);
}

void Engine::update(WriteBuffer *sendbuffer, double frametime)
{
    oldstate = currentstate->clone();
    currentstate->update(sendbuffer, frametime);
}
