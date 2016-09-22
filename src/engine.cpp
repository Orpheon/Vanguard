#include <memory>

#include "engine.h"
#include "entity.h"
#include "gamestate.h"
#include "global_constants.h"

Engine::Engine() : currentstate(this), maskloader(true)
{
    ;// constructor
}

Engine::~Engine()
{
    ;// destructor
}

void Engine::loadmap(std::string mapname)
{
    currentstate.currentmap = std::make_shared<Map>(mapname);
}

void Engine::update(double frametime)
{
    currentstate.update(frametime);
}

PlayerPtr Engine::newplayer()
{
    return currentstate.make_player();
}
