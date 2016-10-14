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
        virtual bool collides(Gamestate *state, EntityPtr otherentity);

        double x, y;
        double hspeed, vspeed;
};

#endif // MOVINGENTITY_H
