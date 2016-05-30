#include <vector>
#include <memory>

#include "gamestate.h"
#include "engine.h"
#include "entity.h"

Gamestate::Gamestate() : entitylist(), currentmap()
{
    time = 0;
}

Gamestate::~Gamestate()
{
    std::vector<Entity*>::iterator i;
    for (i=entitylist.begin(); i!=entitylist.end(); i++)
    {
        delete *i;
    }
}

void Gamestate::update(double frametime)
{
    time += frametime;

    std::vector<Entity*>::iterator i;
    for (i=entitylist.begin(); i!=entitylist.end(); i++)
    {
        (*i)->beginstep(this, frametime);
    }
    for (i=entitylist.begin(); i!=entitylist.end(); i++)
    {
        (*i)->midstep(this, frametime);
    }
    for (i=entitylist.begin(); i!=entitylist.end(); i++)
    {
        (*i)->endstep(this, frametime);
    }
}

Gamestate* Gamestate::clone()
{
    Gamestate *g = new Gamestate();
    g->time = time;
    g->currentmap = currentmap;
    std::vector<Entity*>::iterator i;
    for (i=entitylist.begin(); i!=entitylist.end(); i++)
    {
        g->entitylist.push_back((*i)->clone());
    }
    return g;
}
