#include <cmath>

#include "ingameelements/weapons/peacemaker.h"
#include "renderer.h"
#include "ingameelements/projectiles/peacemakerbullet.h"
#include "ingameelements/heroes/mccree.h"

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
    Mccree *c = state->get<Mccree>(owner);
    if (firinganim.active())
    {
        mainsprite = firinganim.getframe();
    }
    else if (reloadanim.active())
    {
        mainsprite = reloadanim.getframe();
        dir = M_PI*c->isflipped;
    }
    else if (fthanim.active())
    {
        mainsprite = fthanim.getframe();
    }
    else
    {
        mainsprite = c->getcharacterfolder()+"arm/1.png";
    }
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.request_sprite(mainsprite);
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

void Peacemaker::fireprimary(Gamestate *state)
{
    if (clip > 0 and not firinganim.active())
    {
        EntityPtr newshot = state->make_entity<PeacemakerBullet>(state, owner);
        PeacemakerBullet *shot = state->get<PeacemakerBullet>(newshot);
        shot->x = x+std::cos(aimdirection)*10;
        shot->y = y+std::sin(aimdirection)*10;

        shot->hspeed = std::cos(aimdirection) * bulletspeed;
        shot->vspeed = std::sin(aimdirection) * bulletspeed;

        --clip;
        firinganim.reset();
        firinganim.active(true);
    }
}

void Peacemaker::firesecondary(Gamestate *state)
{
    if (clip > 0 and (not isfthing or fthanim.getpercent() >= 1))
    {
        EntityPtr newshot = state->make_entity<PeacemakerBullet>(state, owner);
        PeacemakerBullet *shot = state->get<PeacemakerBullet>(newshot);
        shot->x = x+std::cos(aimdirection)*10;
        shot->y = y+std::sin(aimdirection)*10;
        double spread = (2*(rand()/(RAND_MAX+1.0)) - 1)*40*M_PI/180.0;
        shot->hspeed = std::cos(aimdirection+spread) * bulletspeed;
        shot->vspeed = std::sin(aimdirection+spread) * bulletspeed;

        --clip;
        if (isfthing)
        {
            // We're already in the middle of a fth firing spree
            // Fire on
            fthanim = Animation("heroes/mccree/fanthehammerloop/", std::bind(&Peacemaker::firesecondary, this, state));
        }
        else
        {
            fthanim = Animation("heroes/mccree/fanthehammerstart/", std::bind(&Peacemaker::firesecondary, this, state));
        }
        isfthing = true;
    }
    else if (clip <= 0)
    {
        isfthing = false;
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
