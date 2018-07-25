#include "ingameelements/explosion.h"
#include "renderer.h"

void Explosion::init(uint64_t id_, Gamestate &state, std::string animationpath, double direction_)
{
    MovingEntity::init(id_, state);

    entitytype = ENTITYTYPE::EXPLOSION;
    explosionanim.init(animationpath, std::bind(&Explosion::destroy, this, std::placeholders::_1), true);
    direction = direction_;
}

void Explosion::beginstep(Gamestate &state, double frametime)
{
    explosionanim.update(state, frametime);
}

void Explosion::render(Renderer &renderer, Gamestate &state)
{
    std::string spritepath = explosionanim.getframepath();
    sf::Sprite sprite;
    renderer.spriteloader.loadsprite(spritepath, sprite);
    sprite.setRotation(direction);
    sprite.setPosition(x, y);
    renderer.foreground.draw(sprite);
}
