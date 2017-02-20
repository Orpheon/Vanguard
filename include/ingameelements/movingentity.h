#pragma once

#include "entity.h"
#include "datastructures.h"
#include <string>

// Circular reference
class Gamestate;

class MovingEntity : public Entity
{
    public:
        virtual void init(uint64_t id_, Gamestate *state);
        virtual ~MovingEntity() override = default;
        virtual void endstep(Gamestate *state, double frametime) override;
        virtual std::string getsprite(Gamestate *state, bool mask) = 0;
        virtual void interpolate(Entity *prev_entity, Entity *next_entity, double alpha);
        virtual void serialize(Gamestate *state, WriteBuffer *buffer, bool fullupdate);
        virtual void deserialize(Gamestate *state, ReadBuffer *buffer, bool fullupdate);
        virtual bool collides(Gamestate *state, double testx, double testy);

        double x, y;
        double hspeed, vspeed;
};

