
#include "mapelements/controlpoint.h"
#include "ingameelements/gamemodes/kothmanager.h"
#include "gamestate.h"

void KothManager::init(Rect spawnarea1, Rect spawnarea2, Rect cparea)
{
    GamemodeManager::init(spawnarea1, spawnarea2);
    area = cparea;
}

void KothManager::activate(Gamestate &state, std::function<void(Gamestate &state)> winfunc)
{
    GamemodeManager::activate(state, winfunc);
    controlpoint = 0;
    for (int i=0; i<2; ++i)
    {
        teamcounters[i] = Timer();
        teamcounters[i].init(100, winfunc);
    }
    unlocktimer.init(30, std::bind(&KothManager::createpoint, this, std::placeholders::_1, NO_TEAM));
}

void KothManager::step(Gamestate &state, double frametime)
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

void KothManager::createpoint(Gamestate &state, Team owningteam)
{
    if (state.exists(controlpoint))
    {
        ControlPoint &cp = state.get<ControlPoint>(controlpoint);
        cp.destroy(state);
    }
    controlpoint = state.make_entity<ControlPoint>(area, owningteam, std::bind(&KothManager::createpoint, this,
                                                                               std::placeholders::_1, std::placeholders::_2));
}

void KothManager::destroy(Gamestate &state)
{
    if (state.exists(controlpoint))
    {
        ControlPoint &cp = state.get<ControlPoint>(controlpoint);
        cp.destroy(state);
    }
}