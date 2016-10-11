
#include <cmath>

#include "peacemaker.h"
#include "renderer.h"
#include "peacemakerbullet.h"
#include "mccree.h"

Peacemaker::Peacemaker(uint64_t id_, Gamestate *state, EntityPtr owner_) : Weapon(id_, state, owner_, constructparameters(state))
{
    //ctor
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
        dir = 3.1415*c->animstate()->isflipped;
    }
    else
    {
        mainsprite = "heroes/mccree/arm/1.png";
    }
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.request_sprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->midground);
    if (not c->animstate()->rolling.active())
    {
        if (c->animstate()->isflipped)
        {
            // FIXME What the hell is going on here, this needs to be recalculated properly with paper
            al_draw_scaled_rotated_bitmap(sprite, xoffset, yoffset, x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 1, -1, dir, 0);
        }
        else
        {
            al_draw_rotated_bitmap(sprite, xoffset, yoffset, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, dir, 0);
        }
    }
}

void Peacemaker::fireprimary(Gamestate *state, double frametime)
{
    if (clip > 0 and not firinganim.active())
    {
        EntityPtr newshot = state->make_entity<PeacemakerBullet>(state, owner);
        PeacemakerBullet *shot = state->get<PeacemakerBullet>(newshot);
        shot->x = x;
        shot->y = y+9.0;
        shot->hspeed = std::cos(aimdirection) * bulletspeed;
        shot->vspeed = std::sin(aimdirection) * bulletspeed;

        --clip;
        firinganim.reset();
        firinganim.active(true);
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
