#include <list>

#include "gamestate.h"
#include "engine.h"
#include "entity.h"

Gamestate::Gamestate()
{
    entitylist = new std::list<Entity*>;
    time = 0;
}

Gamestate::~Gamestate()
{
    std::list<Entity*>::iterator i;
    for (i=entitylist->begin(); i!=entitylist->end(); i++)
    {
        delete *i;
    }
    delete entitylist;
}

void Gamestate::update(Engine engine, double frametime)
{
    time += frametime;

    std::list<Entity*>::iterator i;
    for (i=entitylist->begin(); i!=entitylist->end(); i++)
    {
        (*i)->beginstep(engine, frametime);
    }
    for (i=entitylist->begin(); i!=entitylist->end(); i++)
    {
        (*i)->midstep(engine, frametime);
    }
    for (i=entitylist->begin(); i!=entitylist->end(); i++)
    {
        (*i)->endstep(engine, frametime);
    }
}

Gamestate* Gamestate::clone()
{
    Gamestate *g = new Gamestate();
    g->time = time;
    std::list<Entity*>::iterator i;
    for (i=entitylist->begin(); i!=entitylist->end(); i++)
    {
        g->entitylist->push_back((*i)->clone());
    }
    return g;
}
