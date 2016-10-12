
#include <cmath>

#include "ingameelements/projectiles/peacemakerbullet.h"
#include "renderer.h"

PeacemakerBullet::PeacemakerBullet(uint64_t id_, Gamestate *state, EntityPtr owner_) : Projectile::Projectile(id_, state, owner_)
{
    //ctor
}

PeacemakerBullet::~PeacemakerBullet()
{
    //dtor
}

void PeacemakerBullet::midstep(Gamestate *state, double frametime)
{
    if (true)
    {
        if (state->currentmap->collides(Rect(x-2, y, 5, 1)))
        {
            destroyentity = true;
        }
    }
    else
    {
        Projectile::midstep(state, frametime);
    }
}

void PeacemakerBullet::endstep(Gamestate *state, double frametime)
{
    Projectile::endstep(state, frametime);
}

void PeacemakerBullet::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite = getsprite(state, false);
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.request_sprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    double direction = std::atan2(vspeed, hspeed);

    al_set_target_bitmap(renderer->background);
    al_draw_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, direction, 0);
}
