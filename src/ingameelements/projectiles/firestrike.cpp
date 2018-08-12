
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

    double effectivedamage = target.damage(state, DAMAGE, owner, REINHARDT_FIRESTRIKE);
    already_damaged_entities.push_back(target.id);
    return effectivedamage;
}

void Firestrike::render(Renderer &renderer, Gamestate &state)
{
    sf::Sprite sprite;
    renderer.spriteloader.loadsprite(spriteid(), sprite);
    sprite.setRotation(std::atan2(vspeed, hspeed)*180.0/3.1415);
    sprite.setPosition(x, y);
    renderer.background.draw(sprite);;
}