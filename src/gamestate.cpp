#include <unordered_map>
#include <memory>

#include "gamestate.h"
#include "entity.h"
#include "player.h"

Gamestate::Gamestate(Engine *engine_) : engine(engine_), entitylist(), currentmap(), entityidcounter(1)
{
    time = 0;
}

Gamestate::~Gamestate()
{
    ;
}

void Gamestate::update(double frametime)
{
    time += frametime;

    for (auto& e : entitylist)
    {
        if (e.second->isrootobject())
        {
            e.second->beginstep(this, frametime);
        }
    }
    for (auto& e : entitylist)
    {
        if (e.second->isrootobject())
        {
            e.second->midstep(this, frametime);
        }
    }
    for (auto& e : entitylist)
    {
        if (e.second->isrootobject())
        {
            e.second->endstep(this, frametime);
        }
    }
}
