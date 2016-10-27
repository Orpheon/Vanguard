#include <cmath>

#include "ingameelements/weapons/peacemaker.h"
#include "renderer.h"
#include "ingameelements/projectiles/peacemakerbullet.h"
#include "ingameelements/heroes/mccree.h"
#include "ingameelements/explosion.h"
#include "engine.h"

Peacemaker::Peacemaker(uint64_t id_, Gamestate *state, EntityPtr owner_) : Weapon(id_, state, owner_, constructparameters(state)),
                        fthanim("heroes/mccree/fanthehammerstart/", std::bind(&Peacemaker::firesecondary, this, state)), isfthing(false)
{
    fthanim.active(false);
}

Peacemaker::~Peacemaker()
{
    //dtor
}

void Peacemaker::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite;
    double dir = aimdirection;
    Mccree *c = state->get<Mccree>(state->get<Player>(owner)->character);
    if (firinganim.active())
    {
        mainsprite = firinganim.getframe();
    }
    else if (reloadanim.active())
    {
        mainsprite = reloadanim.getframe();
        dir = 3.1415*c->isflipped;
    }
    else if (fthanim.active())
    {
        mainsprite = fthanim.getframe();
    }
    else
    {
        mainsprite = c->getcharacterfolder()+"arm/1";
    }
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->midground);
    if (not c->rollanim.active())
    {
        if (c->isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, -getattachpoint_x()-spriteoffset_x, getattachpoint_y()+spriteoffset_y, x - renderer->cam_x, y - renderer->cam_y, 1, -1, dir, 0);
        }
        else
        {
            al_draw_rotated_bitmap(sprite, getattachpoint_x()+spriteoffset_x, getattachpoint_y()+spriteoffset_y, x - renderer->cam_x, y - renderer->cam_y, dir, 0);
        }
    }
}

void Peacemaker::midstep(Gamestate *state, double frametime)
{
    Weapon::midstep(state, frametime);

    if (isfthing)
    {
        fthanim.active(true);
    }
    fthanim.update(state, frametime);
}

void Peacemaker::reload(Gamestate *state)
{
    if (clip < getclipsize() and not firinganim.active() and not reloadanim.active() and not isfthing)
    {
        // We need to reload
        reloadanim.reset();
        reloadanim.active(true);
    }
}

void Peacemaker::wantfireprimary(Gamestate *state)
{
    if (state->engine->isserver and clip > 0 and not firinganim.active())
    {
        fireprimary(state);
        state->sendbuffer->write<uint8_t>(PRIMARY_FIRED);
        state->sendbuffer->write<uint8_t>(state->findplayerid(owner));
    }
}

void Peacemaker::fireprimary(Gamestate *state)
{
    EntityPtr newshot = state->make_entity<PeacemakerBullet>(state, owner);
    PeacemakerBullet *shot = state->get<PeacemakerBullet>(newshot);
    shot->x = x+std::cos(aimdirection)*10;
    shot->y = y+std::sin(aimdirection)*10;

    shot->hspeed = std::cos(aimdirection) * bulletspeed;
    shot->vspeed = std::sin(aimdirection) * bulletspeed;

    Explosion *e = state->get<Explosion>(state->make_entity<Explosion>(state, "heroes/mccree/projectiletrail/", aimdirection));
    e->x = x+std::cos(aimdirection)*24;
    e->y = y+std::sin(aimdirection)*24;

    --clip;
    firinganim.reset();
    firinganim.active(true);
}

void Peacemaker::wantfiresecondary(Gamestate *state)
{
    if (clip > 0)
    {
        if (not isfthing and state->engine->isserver)
        {
            firesecondary(state);
            state->sendbuffer->write<uint8_t>(SECONDARY_FIRED);
            state->sendbuffer->write<uint8_t>(state->findplayerid(owner));
        }
        else if (isfthing and fthanim.getpercent() >= 1)
        {
            firesecondary(state);
        }
    }
    else
    {
        isfthing = false;
        fthanim.active(false);
    }
}

void Peacemaker::firesecondary(Gamestate *state)
{
    EntityPtr newshot = state->make_entity<PeacemakerBullet>(state, owner);
    PeacemakerBullet *shot = state->get<PeacemakerBullet>(newshot);
    double spread = (2*(rand()/(RAND_MAX+1.0)) - 1)*25*3.1415/180.0;
    shot->x = x+std::cos(aimdirection+spread)*10;
    shot->y = y+std::sin(aimdirection+spread)*10;
    shot->hspeed = std::cos(aimdirection+spread) * bulletspeed;
    shot->vspeed = std::sin(aimdirection+spread) * bulletspeed;

    Explosion *e = state->get<Explosion>(state->make_entity<Explosion>(state, "heroes/mccree/projectiletrail/", aimdirection+spread));
    e->x = x+std::cos(aimdirection+spread)*24;
    e->y = y+std::sin(aimdirection+spread)*24;

    --clip;

    if (clip > 0)
    {
        if (isfthing)
        {
            fthanim = Animation("heroes/mccree/fanthehammerloop/", std::bind(&Peacemaker::wantfiresecondary, this, state));
        }
        else
        {
            fthanim = Animation("heroes/mccree/fanthehammerstart/", std::bind(&Peacemaker::wantfiresecondary, this, state));
            isfthing = true;
        }
    }
}

WeaponChildParameters Peacemaker::constructparameters(Gamestate *state)
{
    WeaponChildParameters p;
    p.clipsize = 6;
    p.characterfolder = "heroes/mccree/";
    p.reloadfunction = std::bind(&Peacemaker::restoreclip, this, state);
    return p;
}
