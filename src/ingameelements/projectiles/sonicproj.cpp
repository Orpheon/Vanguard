
#include "ingameelements/projectiles/sonicproj.h"
#include "renderer.h"

void SonicProj::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Projectile::init(id_, state, owner_);
}

double SonicProj::dealdamage(Gamestate &state, Entity &target)
{
    return target.damage(state, DAMAGE, owner, REGULAR_ATTACK);
}

void SonicProj::render(Renderer &renderer, Gamestate &state)
{
    sf::Sprite sprite;
    renderer.spriteloader.loadsprite(spriteid(), sprite);
    sprite.setRotation(std::atan2(vspeed, hspeed));
    sprite.setPosition(x, y);
    renderer.background.draw(sprite);
}