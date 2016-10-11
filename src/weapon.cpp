#include "weapon.h"
#include "character.h"
#include "gamestate.h"

#include <cmath>

Weapon::Weapon(uint64_t id_, Gamestate *state, EntityPtr owner_, WeaponChildParameters parameters) : MovingEntity(id_, state), owner(owner_), aimdirection(0),
            firinganim(parameters.characterfolder+"firing/"), reloadanim(parameters.characterfolder+"reload/", parameters.reloadfunction), clip(parameters.clipsize)
{
    reloadanim.active(false);
    firinganim.active(false);
}

Weapon::~Weapon()
{
    //dtor
}

void Weapon::beginstep(Gamestate *state, double frametime)
{
    ;
}

void Weapon::midstep(Gamestate *state, double frametime)
{
    if (clip == 0 and not firinganim.active() and not reloadanim.active())
    {
        // We need to reload
        reloadanim.reset();
        reloadanim.active(true);
    }
    reloadanim.update(state, frametime);
    firinganim.update(state, frametime);
}

void Weapon::endstep(Gamestate *state, double frametime)
{
    Character *c = state->get<Character>(owner);
    x = c->x;
    y = c->y;
}

void Weapon::interpolate(Entity *prev_entity, Entity *next_entity, double alpha)
{
//    Weapon *prev_e = static_cast<Weapon*>(prev_entity);
//    Weapon *next_e = static_cast<Weapon*>(next_entity);

    // Todo
}

void Weapon::setaim(double x_, double y_)
{
    aimdirection = std::atan2(y_, x_);
}
