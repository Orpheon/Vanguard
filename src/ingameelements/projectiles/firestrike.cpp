
#include "ingameelements/projectiles/firestrike.h"
#include "renderer.h"

void Firestrike::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Projectile::init(id_, state, owner_);
}

double Firestrike::dealdamage(Gamestate &state, Entity &target)
{
    for (auto& entityptr : already_damaged_entities)
    {
        if (target.id == entityptr.id)
        {
            return 0;
        }
    }

    double effectivedamage = target.damage(state, DAMAGE);
    already_damaged_entities.push_back(target.id);
    return effectivedamage;
}

void Firestrike::render(Renderer &renderer, Gamestate &state)
{
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(spriteid());
    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(spriteid())*renderer.zoom;
    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(spriteid())*renderer.zoom;
    double rel_x = (x - renderer.cam_x)*renderer.zoom;
    double rel_y = (y - renderer.cam_y)*renderer.zoom;

    double direction = std::atan2(vspeed, hspeed);
    al_set_target_bitmap(renderer.background);
    al_draw_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x, rel_y, direction, 0);
}