#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "entity.h"
#include "datastructures.h"
#include <string>

// Circular reference
class Gamestate;

class MovingEntity : public Entity
{
    public:
        MovingEntity(uint64_t id_, Gamestate *state);
        virtual void endstep(Gamestate *state, double frametime);
        virtual std::string getsprite(Gamestate *state, bool mask) = 0;
        virtual void interpolate(Entity *prev_entity, Entity *next_entity, double alpha);
        virtual void serialize(Gamestate *state, WriteBuffer *buffer, bool fullupdate);
        virtual void deserialize(Gamestate *state, ReadBuffer *buffer, bool fullupdate);
        virtual bool collides(Gamestate *state, double testx, double testy);

        double x, y;
        double hspeed, vspeed;
};

#endif // MOVINGENTITY_H
