
#include <cmath>

#include "peacemaker.h"
#include "renderer.h"
#include "peacemakerbullet.h"
#include "mccree.h"

Peacemaker::Peacemaker(uint64_t id_, Gamestate *state, EntityPtr owner_) : Weapon(id_, state, owner_)
{

}

Peacemaker::~Peacemaker()
{
    //dtor
}

void Peacemaker::render(Renderer *renderer, Gamestate *state)
{
    // FIXME: Placeholder
    std::string mainsprite = "heroes/mccree/arm/1.png";
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.request_sprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->midground);

    Mccree *c = state->get<Mccree>(owner);
    if (not c->animstate()->rolling.active())
    {
        if (c->animstate()->isflipped)
        {
            // FIXME What the hell is going on here, this needs to be recalculated properly with paper
            al_draw_scaled_rotated_bitmap(sprite, xoffset, yoffset, x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 1, -1, aimdirection, 0);
        }
        else
        {
            al_draw_rotated_bitmap(sprite, xoffset, yoffset, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, aimdirection, 0);
        }
    }
}

void Peacemaker::fireprimary(Gamestate *state, double frametime)
{
    for (int i=0; i<10; ++i)
    {
        EntityPtr newshot = state->make_entity<PeacemakerBullet>(state, owner);
        PeacemakerBullet *shot = state->get<PeacemakerBullet>(newshot);
        shot->x = x;
        shot->y = y+9.0;
        double dir = aimdirection + 0.3*(2.0*(rand()/(RAND_MAX + 1.0))-1.0);
        shot->hspeed = std::cos(dir) * bulletspeed;
        shot->vspeed = std::sin(dir) * bulletspeed;
    }
}
