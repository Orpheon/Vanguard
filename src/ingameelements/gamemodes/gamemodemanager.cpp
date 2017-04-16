
#include "ingameelements/gamemodes/gamemodemanager.h"
#include "mapelements/spawnroom.h"
#include "gamestate.h"

void GamemodeManager::init(uint64_t id_, Rect spawnarea1, Rect spawnarea2)
{
    Entity::init(id_);
    spawnareas[TEAM1] = spawnarea1;
    spawnareas[TEAM2] = spawnarea2;
}

void GamemodeManager::activate(Gamestate &state, std::function<void(Gamestate &state, Team winners)> winfunc)
{
    spawnrooms[TEAM1] = state.make_entity<Spawnroom>(spawnareas[TEAM1], TEAM1);
    spawnrooms[TEAM2] = state.make_entity<Spawnroom>(spawnareas[TEAM2], TEAM2);
    active = true;
}

void GamemodeManager::destroy(Gamestate &state)
{
    Entity::destroy(state);
    if (state.exists(spawnrooms[TEAM1]))
    {
        Spawnroom &spawn = state.get<Spawnroom>(spawnrooms[TEAM1]);
        spawn.destroy(state);
    }
    if (state.exists(spawnrooms[TEAM2]))
    {
        Spawnroom &spawn = state.get<Spawnroom>(spawnrooms[TEAM2]);
        spawn.destroy(state);
    }
}