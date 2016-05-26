#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "entity.h"
#include "gamestate.h"

class MovingEntity : public Entity
{
    public:
        MovingEntity();
        virtual ~MovingEntity();
        virtual void endstep(Gamestate *state, double frametime) override;

        double x, y;
        double hspeed, vspeed;
};

#endif // MOVINGENTITY_H
