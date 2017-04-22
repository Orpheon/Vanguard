
#include <engine.h>
#include "mapelements/controlpoint.h"
#include "ingameelements/gamemodes/kothmanager.h"
#include "gamestate.h"

void KothManager::init(uint64_t id_, Rect spawnarea1, Rect spawnarea2, Rect cparea)
{
    GamemodeManager::init(id_, spawnarea1, spawnarea2);
    area = cparea;
}

void KothManager::activate(Gamestate &state, std::function<void(Gamestate &state, Team winners)> winfunc_)
{
    GamemodeManager::activate(state, winfunc_);
    winfunc = winfunc_;
    controlpoint = 0;
    for (int i=0; i<2; ++i)
    {
        teamcounters.push_back(Timer());
        teamcounters.at(i).init(100, std::bind(&KothManager::win, this, std::placeholders::_1));
    }
    unlocktimer.init(5, std::bind(&KothManager::createpoint, this, std::placeholders::_1, NO_TEAM));
}

void KothManager::beginstep(Gamestate &state, double frametime)
{
    GamemodeManager::beginstep(state, frametime);

    if (active)
    {
        unlocktimer.update(state, frametime);
        if (state.exists(controlpoint))
        {
            ControlPoint &cp = state.get<ControlPoint>(controlpoint);
            if (cp.owningteam != NO_TEAM)
            {
                teamcounters[cp.owningteam].update(state, frametime);
            }
        }
    }
}

void KothManager::createpoint(Gamestate &state, Team owningteam)
{
    if (state.exists(controlpoint))
    {
        ControlPoint &cp = state.get<ControlPoint>(controlpoint);
        cp.destroy(state);
    }
    controlpoint = state.make_entity<ControlPoint>(area, owningteam, std::bind(&KothManager::createpoint, this,
                                                                               std::placeholders::_1, std::placeholders::_2));
    unlocktimer.active = false;

    if (state.engine.isserver)
    {
        state.engine.sendbuffer.write<uint8_t>(KOTH_CP_CREATED);
        state.engine.sendbuffer.write<uint8_t>(owningteam);
    }
}

void KothManager::destroy(Gamestate &state)
{
    GamemodeManager::destroy(state);

    if (state.exists(controlpoint))
    {
        ControlPoint &cp = state.get<ControlPoint>(controlpoint);
        cp.destroy(state);
    }
}

void KothManager::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    GamemodeManager::interpolate(prev_entity, next_entity, alpha);

    KothManager &prev_e = static_cast<KothManager&>(prev_entity);
    KothManager &next_e = static_cast<KothManager&>(next_entity);

    unlocktimer.interpolate(prev_e.unlocktimer, next_e.unlocktimer, alpha);
    for (unsigned int i=0; i<teamcounters.size(); ++i)
    {
        teamcounters[i].interpolate(prev_e.teamcounters[i], next_e.teamcounters[i], alpha);
    }
}

void KothManager::win(Gamestate &state)
{
    if (teamcounters[TEAM1].timer > teamcounters[TEAM2].timer)
    {
        winfunc(state, TEAM1);
    }
    else if (teamcounters[TEAM1].timer < teamcounters[TEAM2].timer)
    {
        winfunc(state, TEAM2);
    }
    else
    {
        ControlPoint &cp = state.get<ControlPoint>(controlpoint);
        winfunc(state, cp.owningteam);
    }
}

void KothManager::serializefull(WriteBuffer &buffer, Gamestate &state)
{
    buffer.write<double>(teamcounters.at(TEAM1).timer);
    buffer.write<double>(teamcounters.at(TEAM2).timer);
    buffer.write<bool>(state.exists(controlpoint));
    if (state.exists(controlpoint))
    {
        ControlPoint &cp = state.get<ControlPoint>(controlpoint);
        cp.serializefull(buffer, state);
    }
}

void KothManager::deserializefull(ReadBuffer &buffer, Gamestate &state)
{
    teamcounters.at(TEAM1).timer = buffer.read<double>();
    teamcounters.at(TEAM2).timer = buffer.read<double>();
    bool cpexists = buffer.read<bool>();
    if (cpexists)
    {
        createpoint(state, NO_TEAM);
        ControlPoint &cp = state.get<ControlPoint>(controlpoint);
        cp.deserializefull(buffer, state);
    }
}