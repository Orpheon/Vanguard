#pragma once

#include "ingameelements/gamemodes/gamemodemanager.h"
#include "timer.h"

class KothManager : public GamemodeManager
{
    public:
        virtual void init(Rect spawnarea1, Rect spawnarea2, Rect cparea);
        virtual void activate(Gamestate &state, std::function<void(Gamestate &state)> winfunc) override;
        virtual void serialize(WriteBuffer &buffer, Gamestate &state) override {}
        virtual void deserialize(ReadBuffer &buffer, Gamestate &state) override {}
        virtual void step(Gamestate &state, double frametime) override;
        virtual void destroy(Gamestate &state) override;
        virtual void createpoint(Gamestate &state, Team owningteam);

        Timer unlocktimer;
        std::vector<Timer> teamcounters;
        Rect area;
        EntityPtr controlpoint;
};