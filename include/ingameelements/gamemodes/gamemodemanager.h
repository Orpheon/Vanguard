#pragma once
#include "datastructures.h"

class GamemodeManager
{
    public:
        virtual void init(Rect spawnarea1, Rect spawnarea2);
        virtual void activate(Gamestate &state, std::function<void(Gamestate &state)> winfunc);
        virtual void serialize(WriteBuffer &buffer, Gamestate &state) = 0;
        virtual void deserialize(ReadBuffer &buffer, Gamestate &state) = 0;
        virtual void step(Gamestate &state, double frametime) = 0;
        virtual void destroy(Gamestate &state);
        std::vector<Rect> spawnareas;
        std::vector<EntityPtr> spawnrooms;
};