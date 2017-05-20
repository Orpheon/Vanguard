#include "ingameelements/projectiles/flashbang.h"
#include "renderer.h"
#include "ingameelements/explosion.h"
#include "engine.h"

#include <functional>

void Flashbang::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Projectile::init(id_, state, owner_);

    countdown.init(0.3, std::bind(&Flashbang::destroy, this, std::placeholders::_1));
}

void Flashbang::beginstep(Gamestate &state, double frametime)
{
    Projectile::beginstep(state, frametime);
    countdown.update(state, frametime);
}

void Flashbang::render(Renderer &renderer, Gamestate &state)
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

double Flashbang::explode(Gamestate &state)
{
    Explosion &explosion = state.get<Explosion>(state.make_entity<Explosion>(state, "heroes/mccree/flashbang_explosion/", 0));
    explosion.x = x;
    explosion.y = y;
    double dmgdealt = 0;
    
    for (auto &e : state.entitylist)
    {
        auto &entity = *(e.second);
        if (not entity.destroyentity)
        {
            if (entity.hasposition() and entity.damageableby(team))
            {
                double collisionptx, collisionpty;
                MovingEntity &mv = static_cast<MovingEntity&>(entity);
                if (std::hypot(mv.x - x, mv.y - y) < EXPLOSION_RADIUS)
                {
                    EntityPtr target = state.collidelinetarget(state, x, y, mv, team, PENETRATE_CHARACTER,
                                                               &collisionptx, &collisionpty);
                    if (target.id == entity.id)
                    {
                        entity.damage(state, 25);
                        dmgdealt += 25;
                        entity.stun(state);
                    }
                }
            }
        }
    }

    return dmgdealt;
}

void Flashbang::destroy(Gamestate &state)
{
    double dmgdealt = explode(state);
    // TODO: Register ult damage
    Projectile::destroy(state);
}