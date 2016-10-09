#include "weapon.h"

#include <cmath>

Weapon::Weapon(Gamestate *state, EntityPtr owner_) : MovingEntity(state), owner(owner_), aimdirection(0)
{
    //ctor
}

Weapon::~Weapon()
{
    //dtor
}

void Weapon::setaim(double x, double y)
{
    aimdirection = std::atan2(y, x);
}
