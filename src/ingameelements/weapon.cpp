#include "ingameelements/weapon.h"
#include "ingameelements/character.h"
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
    if (clip == 0)
    {
        reload(state, frametime);
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
    MovingEntity::interpolate(prev_entity, next_entity, alpha);

    Weapon *prev_e = static_cast<Weapon*>(prev_entity);
    Weapon *next_e = static_cast<Weapon*>(next_entity);

    if (alpha < 0.5)
    {
        clip = prev_e->clip;
    }
    else
    {
        clip = next_e->clip;
    }

    reloadanim.interpolate(&(prev_e->reloadanim), &(next_e->reloadanim), alpha);
    firinganim.interpolate(&(prev_e->firinganim), &(next_e->firinganim), alpha);

    aimdirection = prev_e->aimdirection + alpha*(next_e->aimdirection - prev_e->aimdirection);
}

void Weapon::reload(Gamestate *state, double frametime)
{
    if (clip < getclipsize() and not firinganim.active() and not reloadanim.active())
    {
        // We need to reload
        reloadanim.reset();
        reloadanim.active(true);
    }
}

void Weapon::setaim(double x_, double y_)
{
    aimdirection = std::atan2(y_, x_);
}

void Weapon::serialize(Gamestate *state, WriteBuffer *buffer)
{
    // Hopefully no clip size goes above 255
    buffer->write<uint8_t>(clip);
}

void Weapon::deserialize(Gamestate *state, ReadBuffer *buffer)
{
    // Hopefully no clip size goes above 255
    clip = buffer->read<uint8_t>();
}
