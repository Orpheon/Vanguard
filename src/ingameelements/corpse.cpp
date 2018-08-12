#include "ingameelements/corpse.h"

#include "gamestate.h"
#include "engine.h"
#include "renderer.h"

void Corpse::init(uint64_t id_, Gamestate &state, std::string herofolder, bool isflipped_)
{
    MovingEntity::init(id_, state);

    spritepath = herofolder+"corpse/1";
    countdown.init(8, std::bind(&Corpse::destroy, this, std::placeholders::_1), true);
    isflipped = isflipped_;
}

void Corpse::beginstep(Gamestate &state, double frametime)
{
    countdown.update(state, frametime);

    // Gravity
    vspeed += 540.0*frametime;

    // Collision with wallmask
    if (state.currentmap->collides(state.engine.maskloader.get_rect(spritepath).offset(x, y)))
    {
        vspeed = 0;
    }
}

void Corpse::render(Renderer &renderer, Gamestate &state)
{
    sf::Sprite sprite;
    renderer.spriteloader.loadsprite(spritepath, sprite);

    if (isflipped)
    {
        // Flip horizontally
        sprite.setScale(sf::Vector2f(-1, 1));
    }
    renderer.background.draw(sprite);
}

void Corpse::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Corpse &prev_e = static_cast<Corpse&>(prev_entity);
    Corpse &next_e = static_cast<Corpse&>(next_entity);

    countdown.timer = prev_e.countdown.timer + alpha*(next_e.countdown.timer - prev_e.countdown.timer);
}
