#include "movingentity.h"
#include "gamestate.h"
#include "entity.h"

MovingEntity::MovingEntity(Gamestate *state) : Entity(state)
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

void MovingEntity::clonedata(MovingEntity* m)
{
    m->x = x;
    m->y = y;
    m->hspeed = hspeed;
    m->vspeed = vspeed;
}
