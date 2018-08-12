#include "ingameelements/projectiles/flashbang.h"
#include "renderer.h"
#include "ingameelements/explosion.h"
#include "engine.h"

#include <functional>

void Flashbang::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Projectile::init(id_, state, owner_);

    countdown.init(0.3, std::bind(&Flashbang::destroy, this, std::placeholders::_1), true);
}

void Flashbang::beginstep(Gamestate &state, double frametime)
{
    Projectile::beginstep(state, frametime);
    countdown.update(state, frametime);
}

void Flashbang::render(Renderer &renderer, Gamestate &state)
{
    sf::Sprite sprite;
    renderer.spriteloader.loadsprite(spriteid(), sprite);
    sprite.setRotation(std::atan2(vspeed, hspeed)*180.0/3.1415);
    sprite.setPosition(x, y);
    renderer.background.draw(sprite);
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
                MovingEntity &mv = static_cast<MovingEntity&>(entity);
                double centerx, centery;
                double maxdamagedist = mv.maxdamageabledist(state, &centerx, &centery);
                if (std::hypot(centerx - x, centery - y) < EXPLOSION_RADIUS + maxdamagedist)
                {
                    EntityPtr target = state.collidelineshielded(x, y, centerx, centery, mv, team, PENETRATE_CHARACTER);
                    if (target.id == entity.id)
                    {
                        entity.damage(state, 25, owner, MCCREE_FLASHBANG);
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
    state.get<Player&>(owner).registerdamage(state, dmgdealt);
    Projectile::destroy(state);
}