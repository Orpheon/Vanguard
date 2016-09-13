#include <unordered_map>
#include <memory>

#include "gamestate.h"
#include "engine.h"
#include "entity.h"
#include "player.h"

Gamestate::Gamestate() : entitylist(), playerlist(), currentmap(), entityidcounter(0), playeridcounter(0)
{
    time = 0;
}

Gamestate::~Gamestate()
{
    ;
}

PlayerPtr Gamestate::make_player()
{
    uint64_t id = playeridcounter++;
    playerlist[id] = std::unique_ptr<Player>(new Player(this));
    playerlist[id]->id = id;
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

    for (auto& e : entitylist)
    {
        e.second->beginstep(this, frametime);
    }
    for (auto& p : playerlist)
    {
        p.second->midstep(this, frametime);
    }
    for (auto& e : entitylist)
    {
        e.second->midstep(this, frametime);
    }
    for (auto& e : entitylist)
    {
        e.second->endstep(this, frametime);
    }
}
