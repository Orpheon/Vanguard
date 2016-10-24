#include "ingameelements/projectile.h"

#include <cmath>

Projectile::Projectile(double id_, Gamestate *state, EntityPtr owner_) : MovingEntity(id_, state), owner(owner_)
{
    //ctor
}

Projectile::~Projectile()
{
    //dtor
}

void Projectile::midstep(Gamestate *state, double frametime)
{
    if (isrectangular())
    {
        if (state->currentmap->collides(getrect(), std::atan2(vspeed, hspeed)))
        {
            destroy(state);
        }
    }
}
