#include "ingameelements/clipweapon.h"

Clipweapon::Clipweapon(uint64_t id_, Gamestate *state, EntityPtr owner_, WeaponChildParameters parameters) : Weapon(id_, state, owner_, parameters),
            reloadanim(parameters.characterfolder+"reload/", parameters.reloadfunction), clip(parameters.clipsize)
{
    reloadanim.active(false);
}

Clipweapon::~Clipweapon()
{
    //dtor
}

void Clipweapon::midstep(Gamestate *state, double frametime)
{
    if (clip == 0)
    {
        reload(state);
    }
    reloadanim.update(state, frametime);
}

void Clipweapon::interpolate(Entity *prev_entity, Entity *next_entity, double alpha)
{
    Weapon::interpolate(prev_entity, next_entity, alpha);

    Clipweapon *prev_e = static_cast<Clipweapon*>(prev_entity);
    Clipweapon *next_e = static_cast<Clipweapon*>(next_entity);

    if (alpha < 0.5)
    {
        clip = prev_e->clip;
    }
    else
    {
        clip = next_e->clip;
    }

    reloadanim.interpolate(&(prev_e->reloadanim), &(next_e->reloadanim), alpha);
}

void Clipweapon::reload(Gamestate *state)
{
    if (clip < getclipsize() and not firinganim.active() and not reloadanim.active())
    {
        // We need to reload
        reloadanim.reset();
        reloadanim.active(true);
    }
}

void Clipweapon::serialize(Gamestate *state, WriteBuffer *buffer, bool fullupdate)
{
    Weapon::serialize(state, buffer, fullupdate);

    // Hopefully no clip size goes above 255
    buffer->write<uint8_t>(clip);
}

void Clipweapon::deserialize(Gamestate *state, ReadBuffer *buffer, bool fullupdate)
{
    Weapon::deserialize(state, buffer, fullupdate);

    // Hopefully no clip size goes above 255
    clip = buffer->read<uint8_t>();
}
