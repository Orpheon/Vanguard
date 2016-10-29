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
    currentstate->currentmap = std::make_shared<Map>(mapname);
}

void Engine::update(WriteBuffer *sendbuffer, double frametime)
{
    oldstate = currentstate->clone();
    currentstate->update(sendbuffer, frametime);
}

void Engine::setinput(EntityPtr myself, InputContainer held_keys, double mouse_x, double mouse_y)
{
    Player *p = currentstate->get<Player>(myself);
    if (p != 0)
    {
        Character *c = p->getcharacter(currentstate.get());
        if (c != 0)
        {
            c->setinput(currentstate.get(), held_keys, mouse_x, mouse_y);
        }
    }
}
