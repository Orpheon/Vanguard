#include <unordered_map>
#include <memory>

#include "gamestate.h"
#include "entity.h"
#include "player.h"

Gamestate::Gamestate(Engine *engine_) : entitylist(), currentmap(), engine(engine_), entityidcounter(1)
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
        if (e.second->isrootobject() and not e.second->destroyentity)
        {
            e.second->beginstep(this, frametime);
        }
    }
    for (auto& e : entitylist)
    {
        if (e.second->isrootobject() and not e.second->destroyentity)
        {
            e.second->midstep(this, frametime);
        }
    }
    for (auto& e : entitylist)
    {
        if (e.second->isrootobject() and not e.second->destroyentity)
        {
            e.second->endstep(this, frametime);
        }
    }
    for (auto e = entitylist.begin(); e != entitylist.end();)
    {
        if (e->second->destroyentity)
        {
            e = entitylist.erase(e);
        }
        else
        {
            ++e;
        }
    }
}

std::unique_ptr<Gamestate> Gamestate::clone()
{
    std::unique_ptr<Gamestate> g = std::unique_ptr<Gamestate>(new Gamestate(engine));
    for (auto& e : entitylist)
    {
        g->entitylist[e.second->id] = e.second->clone();
    }
    g->time = time;
    g->entityidcounter = entityidcounter;
    g->currentmap = currentmap;
    return g;
}

void Gamestate::interpolate(Gamestate *prevstate, Gamestate *nextstate, double alpha)
{
    // Use threshold of alpha=0.5 to decide binary choices like entity existence
    Gamestate *preferredstate;
    if (alpha < 0.5)
    {
        preferredstate = prevstate;
    }
    else
    {
        preferredstate = nextstate;
    }
    currentmap = preferredstate->currentmap;

    entitylist.clear();
    for (auto& e : preferredstate->entitylist)
    {
        entitylist[e.second->id] = e.second->clone();
        if (prevstate->entitylist.count(e.second->id) != 0 && nextstate->entitylist.count(e.second->id) != 0)
        {
            // If the instance exists in both states, we need to actually interpolate values
            entitylist[e.second->id]->interpolate(prevstate->entitylist[e.second->id].get(), nextstate->entitylist[e.second->id].get(), alpha);
        }
    }
}
