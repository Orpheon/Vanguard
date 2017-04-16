#pragma once
#include "datastructures.h"
#include "entity.h"

class GamemodeManager : public Entity
{
    public:
        virtual void init(uint64_t id_, Rect spawnarea1, Rect spawnarea2);
        virtual void activate(Gamestate &state, std::function<void(Gamestate &state, Team winners)> winfunc);
        virtual void serialize(WriteBuffer &buffer, Gamestate &state) = 0;
        virtual void deserialize(ReadBuffer &buffer, Gamestate &state) = 0;
        virtual void destroy(Gamestate &state) override;
        std::vector<Rect> spawnareas;
        std::vector<EntityPtr> spawnrooms;
        bool active = false;
};