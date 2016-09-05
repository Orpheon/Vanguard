#include <unordered_map>
#include <memory>

#include "gamestate.h"
#include "engine.h"
#include "entity.h"
#include "player.h"

Gamestate::Gamestate() : entitylist(), currentmap(), entityidcounter(0), playeridcounter(0)
{
    time = 0;
}

Gamestate::~Gamestate()
{
    ;
}

template<class EntityT, class ...Args>
EntityPtr Gamestate::make_entity(Args&& ...args)
{
    uint64_t id = entityidcounter++;
    entitylist[id] = new std::unique_ptr<Entity>(new EntityT(std::forward<Args>(args)...));
    return EntityPtr(id);
}

PlayerPtr Gamestate::make_player()
{
    uint64_t id = playeridcounter++;
    playerlist[id] = new std::unique_ptr<Player>(new Player(this));
    return PlayerPtr(id);
}

Entity* Gamestate::get(EntityPtr e)
{
    return entitylist[e.id].get();
}

Player* Gamestate::get(PlayerPtr p)
{
    return playerlist[p.id].get();
}

void Gamestate::update(double frametime)
{
    time += frametime;

    for (auto e : entitylist)
    {
        e.second->beginstep(this, frametime);
    }
    for (auto p : playerlist)
    {
        p.second->midstep(this, frametime);
    }
    for (auto e : entitylist)
    {
        e.second->midstep(this, frametime);
    }
    for (auto e : entitylist)
    {
        e.second->endstep(this, frametime);
    }
}

Gamestate* Gamestate::clone()
{
    Gamestate *g = new Gamestate();
    g->time = time;
    g->currentmap = currentmap;

    for (auto e : entitylist)
    {
        e.second->clone(this, g);
    }
    for (auto p : playerlist)
    {
        p.second->clone(this, g);
    }

    return g;
}
