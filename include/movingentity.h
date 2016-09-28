#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include "entity.h"
#include <string>

// Circular reference
class Gamestate;

class MovingEntity : public Entity
{
    public:
        MovingEntity(Gamestate *state);
        virtual void endstep(Gamestate *state, double frametime);

        double x, y;
        double hspeed, vspeed;
        bool isflipped;
};

#endif // MOVINGENTITY_H
