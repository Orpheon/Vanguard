#include "ingameelements/weapon.h"
#include "ingameelements/character.h"
#include "gamestate.h"
#include "engine.h"

#include <cmath>

Weapon::Weapon(uint64_t id_, Gamestate *state, EntityPtr owner_, WeaponChildParameters parameters) : MovingEntity(id_, state), owner(owner_), aimdirection(0), team(state->get<Player>(owner)->team),
            firinganim(parameters.characterfolder+"firing/"), reloadanim(parameters.characterfolder+"reload/", parameters.reloadfunction), clip(parameters.clipsize)
{
    reloadanim.active(false);
    firinganim.active(false);

    entitytype = WEAPON;
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
        reload(state);
    }
    reloadanim.update(state, frametime);
    firinganim.update(state, frametime);
}

void Weapon::endstep(Gamestate *state, double frametime)
{
    Character *c = state->get<Player>(owner)->getcharacter(state);
    double xoffset=0, yoffset=0;
    if (c->weaponvisible(state))
    {
        xoffset = state->engine->maskloader.getweaponoffset_x(c->getsprite(state, false));
        yoffset = state->engine->maskloader.getweaponoffset_y(c->getsprite(state, false));
    }
    x = c->x + xoffset*(c->isflipped ? -1:1);
    y = c->y + yoffset;
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

void Weapon::reload(Gamestate *state)
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
    aimdirection = std::atan2(y_-y, x_-x);
}

void Weapon::serialize(Gamestate *state, WriteBuffer *buffer, bool fullupdate)
{
    // Hopefully no clip size goes above 255
    buffer->write<uint8_t>(clip);
}

void Weapon::deserialize(Gamestate *state, ReadBuffer *buffer, bool fullupdate)
{
    // Hopefully no clip size goes above 255
    clip = buffer->read<uint8_t>();
}
