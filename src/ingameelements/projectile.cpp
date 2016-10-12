#include "ingameelements/projectile.h"

Projectile::Projectile(double id_, Gamestate *state, EntityPtr owner_) : MovingEntity(id_, state), owner(owner_)
{
    //ctor
}

Projectile::~Projectile()
{
    //dtor
}

void Projectile::beginstep(Gamestate *state, double frametime)
{
    ;
}

void Projectile::midstep(Gamestate *state, double frametime)
{
    // Check collision with wallmask
    if (state->currentmap->collides(state, this))
    {
        destroyentity = true;
    }
}

void Projectile::endstep(Gamestate *state, double frametime)
{
    MovingEntity::endstep(state, frametime);
}
