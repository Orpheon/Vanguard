
#include "engine.h"
#include "ingameelements/heroes/soldier76.h"
#include "colorpalette.h"

void Soldier76::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Character::init(id_, state, owner_);
}

void Soldier76::render(Renderer &renderer, Gamestate &state)
{
    Character::render(renderer, state);
//    state.get<Pulserifle&>(weapon).renderbehind(renderer, state);

    std::string spritepath;
    sf::Sprite sprite;

    spritepath = currentsprite(state, false);
    renderer.spriteloader.loadsprite(spritepath, sprite);
    sprite.setPosition(x, y);
    if (isflipped)
    {
        sprite.setScale(-1, 1);
    }
    renderer.midground.draw(sprite);
    if (state.get<Player&>(renderer.myself).team != team)
    {
        // Draw enemy outline
        sprite.setColor(COLOR_ENEMY_OUTLINE);
        renderer.spriteloader.loadspriteoutline(spritepath, sprite);
        renderer.midground.draw(sprite);
        sprite.setColor(sf::Color::White);
    }

    state.get<Weapon>(weapon).render(renderer, state);
}

void Soldier76::beginstep(Gamestate &state, double frametime)
{
    Character::beginstep(state, frametime);

    if (canuseabilities(state) and state.engine.isserver)
    {
//        if (heldkeys.ABILITY_1 and not crossfadeheal.active() and not crossfadespeed.active())
//        {
//            useability1(state);
//            state.engine.sendbuffer.write<uint8_t>(ABILITY1_USED);
//            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
//        }
//
//        if (heldkeys.ABILITY_2 and not ampitupcooldown.active)
//        {
//            useability2(state);
//            state.engine.sendbuffer.write<uint8_t>(ABILITY2_USED);
//            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
//        }
    }
}

void Soldier76::midstep(Gamestate &state, double frametime)
{
    Character::midstep(state, frametime);
}

void Soldier76::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Character::interpolate(prev_entity, next_entity, alpha);

    Soldier76 &p = static_cast<Soldier76&>(prev_entity);
    Soldier76 &n = static_cast<Soldier76&>(next_entity);
}

void Soldier76::useability1(Gamestate &state)
{
//    if (currentaura == HEALAURA)
//    {
//        crossfadespeed.reset();
//        currentaura = SPEEDAURA;
//    }
//    else
//    {
//        crossfadeheal.reset();
//        currentaura = HEALAURA;
//    }
}

void Soldier76::useability2(Gamestate &state)
{
//    ampitup.reset();
//    ampitupcooldown.reset();
//    if (heldkeys.LEFT or heldkeys.RIGHT or not onground(state))
//    {
//        ampitupbackarm.reset();
//    }
//    else
//    {
//        ampitupstanding.reset();
//    }
}

void Soldier76::useultimate(Gamestate &state)
{
//    soundbarrier.reset();
}

void Soldier76::interrupt(Gamestate &state)
{

}

bool Soldier76::weaponvisible(Gamestate &state)
{
    return Character::weaponvisible(state);
}

Rect Soldier76::getcollisionrect(Gamestate &state)
{
    if (crouchanim.active())
    {
        return state.engine.maskloader.get_json_rect(herofolder()+"crouch/").offset(x, y);
    }
    return getstandingcollisionrect(state);
}

Rect Soldier76::getstandingcollisionrect(Gamestate &state)
{
    return state.engine.maskloader.get_json_rect(herofolder()).offset(x, y);
}

std::string Soldier76::currentsprite(Gamestate &state, bool mask)
{
    if (pinanim.active())
    {
        return pinanim.getframepath();
    }
    if (stunanim.active())
    {
        return stunanim.getframepath();
    }
    if (earthshatteredfallanim.active())
    {
        return earthshatteredfallanim.getframepath();
    }
    if (earthshatteredanim.active())
    {
        return earthshatteredanim.getframepath();
    }
    if (earthshatteredgetupanim.active())
    {
        return earthshatteredgetupanim.getframepath();
    }
    if (crouchanim.active())
    {
        return crouchanim.getframepath();
    }
    if (not ongroundsmooth.active)
    {
        if (vspeed < 0)
        {
            return herofolder()+"jump/1";
        }
        else
        {
            return herofolder()+"jumpdrop/1";
        }
    }
    if (std::fabs(hspeed) < 11.0 and not heldkeys.LEFT and not heldkeys.RIGHT)
    {
        return herofolder()+"idle/1";
    }
    return runanim.getframepath();
}