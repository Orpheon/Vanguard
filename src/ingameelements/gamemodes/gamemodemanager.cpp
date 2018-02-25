
#include "ingameelements/gamemodes/gamemodemanager.h"
#include "mapelements/spawnroom.h"
#include "gamestate.h"

void GamemodeManager::init(uint64_t id_, std::vector<Rect> &spawnareas1, std::vector<Rect> &spawnareas2)
{
    Entity::init(id_);
    spawnareas = {spawnareas1, spawnareas2};
}

void GamemodeManager::activate(Gamestate &state, std::function<void(Gamestate &state, Team winners)> winfunc)
{
    spawnrooms = {
        state.make_entity<Spawnroom>(spawnareas.at(TEAM1), TEAM1),
        state.make_entity<Spawnroom>(spawnareas.at(TEAM2), TEAM2)
    };
    active = true;
}

void GamemodeManager::destroy(Gamestate &state)
{
    Entity::destroy(state);
    if (state.exists(spawnrooms.at(TEAM1)))
    {
        Spawnroom &spawn = state.get<Spawnroom>(spawnrooms.at(TEAM1));
        spawn.destroy(state);
    }
    if (state.exists(spawnrooms.at(TEAM2)))
    {
        Spawnroom &spawn = state.get<Spawnroom>(spawnrooms.at(TEAM2));
        spawn.destroy(state);
    }
}
