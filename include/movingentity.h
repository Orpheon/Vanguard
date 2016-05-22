#ifndef MOVINGENTITY_H
#define MOVINGENTITY_H

#include <entity.h>


class MovingEntity : public Entity
{
    public:
        MovingEntity();
        virtual void beginstep(Gamestate *state, double frametime) = 0;
        virtual void midstep(Gamestate *state, double frametime) = 0;
        virtual void endstep(Gamestate *state, double frametime);
        virtual void render() = 0;
        virtual MovingEntity* clone() = 0;

        double x, y;
        double hspeed, vspeed;
};

#endif // MOVINGENTITY_H
