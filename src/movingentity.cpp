#include "movingentity.h"
#include "gamestate.h"

MovingEntity::MovingEntity()
{
    x = 0;
    y = 0;
    hspeed = 0;
    vspeed = 0;
}

void MovingEntity::endstep(Gamestate *state, double frametime)
{
    x += hspeed*frametime;
    y += vspeed*frametime;
}
