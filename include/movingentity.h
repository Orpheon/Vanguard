#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "entity.h"

// Circular reference
class Gamestate;

class MovingEntity : public Entity
{
    public:
        MovingEntity(Gamestate *state);
        virtual void endstep(Gamestate *state, double frametime);
        virtual void clonedata(MovingEntity* m);

        double x, y;
        double hspeed, vspeed;
};

#endif // MOVINGENTITY_H
