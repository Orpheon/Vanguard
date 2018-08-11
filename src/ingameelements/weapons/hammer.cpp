#include <cmath>

#include "ingameelements/weapons/hammer.h"
#include "renderer.h"
#include "ingameelements/heroes/reinhardt.h"
#include "ingameelements/projectiles/firestrike.h"
#include "colorpalette.h"

#include "engine.h"

void Hammer::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Weapon::init(id_, state, owner_);

    barrierptr = state.make_entity<ReinhardtShield>(state, team, owner_);

    firestrikeanim.init(herofolder()+"firestrikebackarm/", false);
    firestrikedelay.init(firestrikeanim.timer.duration * 0.5,
                         std::bind(&Hammer::createfirestrike, this, std::placeholders::_1), false);
    firestrikecooldown.init(6, false);

    // 6th and 14th frame of an assumed 20 frame animation - ugly but idk better way
    firingdelay1.init(firinganim.timer.duration * 6.0/20.0, std::bind(&Hammer::hitarea, this, std::placeholders::_1),
                      false);
    firingdelay2.init(firinganim.timer.duration * 14.0/20.0, std::bind(&Hammer::hitarea, this, std::placeholders::_1),
                      false);
}

void Hammer::renderbehind(Renderer &renderer, Gamestate &state)
{
    Reinhardt &reinhardt = state.get<Reinhardt>(state.get<Player>(owner).character);
    if (reinhardt.weaponvisible(state) and not firinganim.active())
    {
        std::string spritepath;
        sf::Sprite sprite;
        if (firestrikeanim.active())
        {
            spritepath = firestrikeanim.getframepath();
            if (reinhardt.isflipped)
            {
                sprite.setScale(-1, 1);
            }
        }
        else if (barrier(state).active)
        {
            spritepath = herofolder()+"shield/back";
            sprite.setRotation(aimdirection * 180.0/3.1415);
            if (reinhardt.isflipped)
            {
                sprite.setScale(1, -1);
            }
        }
        else
        {
            spritepath = herofolder()+"arm/back";
            if (reinhardt.isflipped)
            {
                sprite.setScale(-1, 1);
            }
        }


        renderer.spriteloader.loadsprite(spritepath, sprite);
        sprite.setPosition(x - getbackattachpoint_x(state), y - getbackattachpoint_y(state));
        renderer.midground.draw(sprite);

        if (state.get<Player&>(renderer.myself).team != team)
        {
            // Draw enemy outline
            sprite.setColor(COLOR_ENEMY_OUTLINE);
            renderer.spriteloader.loadspriteoutline(spritepath, sprite);
            renderer.midground.draw(sprite);
            sprite.setColor(sf::Color::White);
        }
    }
}

void Hammer::render(Renderer &renderer, Gamestate &state)
{
    Reinhardt &reinhardt = state.get<Reinhardt>(state.get<Player>(owner).character);
    if (reinhardt.weaponvisible(state))
    {
        std::string spritepath;
        if (firinganim.active())
        {
            spritepath = firinganim.getframepath();
        }
        else if (firestrikeanim.active())
        {
            spritepath = herofolder()+"firestrikefrontarm/"+std::to_string(firestrikeanim.getframe());
        }
        else if (barrier(state).active)
        {
            spritepath = herofolder()+"shield/front";

        }
        else
        {
            spritepath = herofolder()+"arm/front";
        }
        sf::Sprite sprite;
        renderer.spriteloader.loadsprite(spritepath, sprite);
        sprite.setPosition(x-getattachpoint_x(state), y-getattachpoint_y(state));
        if (reinhardt.isflipped)
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

        barrier(state).render(renderer, state);
    }
}

void Hammer::beginstep(Gamestate &state, double frametime)
{
    Weapon::beginstep(state, frametime);
    barrier(state).beginstep(state, frametime);
    firestrikeanim.update(state, frametime);
    firestrikedelay.update(state, frametime);
    firestrikecooldown.update(state, frametime);
    firingdelay1.update(state, frametime);
    firingdelay2.update(state, frametime);
}

void Hammer::midstep(Gamestate &state, double frametime)
{
    Weapon::midstep(state, frametime);
    barrier(state).midstep(state, frametime);
}

void Hammer::endstep(Gamestate &state, double frametime)
{
    Weapon::endstep(state, frametime);
    barrier(state).endstep(state, frametime);
}

void Hammer::wantfireprimary(Gamestate &state)
{
    if (state.engine.isserver and not firinganim.active() and not firestrikeanim.active())
    {
        fireprimary(state);
        state.engine.sendbuffer.write<uint8_t>(PRIMARY_FIRED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
    }
}

void Hammer::fireprimary(Gamestate &state)
{
    firinganim.reset();
    firinganim.active(true);
    firingdelay1.reset();
    firingdelay1.active = true;
    firingdelay2.reset();
    firingdelay2.active = true;
}

void Hammer::wantfiresecondary(Gamestate &state)
{
    // Do nothing
}

void Hammer::firesecondary(Gamestate &state)
{
    // Do nothing
}

void Hammer::createfirestrike(Gamestate &state)
{
    Firestrike &firestrike = state.get<Firestrike&>(state.make_entity<Firestrike>(state, owner));
    firestrike.x = x + std::cos(aimdirection) * 40;
    firestrike.y = y + std::sin(aimdirection) * 40;
    firestrike.hspeed = firestrike.SPEED * std::cos(aimdirection);
    firestrike.vspeed = firestrike.SPEED * std::sin(aimdirection);

    if (state.currentmap->collideline(x, y, firestrike.x, firestrike.y))
    {
        firestrike.destroy(state);
    }

    firestrikecooldown.reset();
}

void Hammer::hitarea(Gamestate &state)
{
    Reinhardt &reinhardt = state.get<Reinhardt>(state.get<Player>(owner).character);
    for (auto &e : state.entitylist)
    {
        auto &entity = *(e.second);
        if (not entity.destroyentity)
        {
            if (entity.damageableby(team))
            {
                int direction = (reinhardt.isflipped ? -1 : 1);
                bool foundcollision = false;
                for (int i=0; i<110 and not foundcollision; ++i)
                {
                    for (int j=0; j<60 and not foundcollision; ++j)
                    {
                        if (entity.collides(state, x + direction*(-20 + i), y - 30 + j))
                        {
                            // We hit something, check if it's protected
                            if (state.collidelineshielded(x, y, x + direction*(-20 + i), y - 30 + j,
                                                          state.get<MovingEntity&>(entity.id), team,
                                                          static_cast<PenetrationLevel>(PENETRATE_CHARACTER | PENETRATE_SHIELD)).id == entity.id)
                            {
                                double effectivedamage = entity.damage(state, DAMAGE, owner, REGULAR_ATTACK);
                                state.get<Player&>(owner).registerdamage(state, effectivedamage);
                            }
                            foundcollision = true;
                        }
                    }
                }
            }
        }
    }
}

double Hammer::getattachpoint_x(Gamestate &state)
{
    return 0;
}

double Hammer::getattachpoint_y(Gamestate &state)
{
    return 8;
}

double Hammer::getbackattachpoint_x(Gamestate &state)
{
    if (barrier(state).active)
    {
        return 6 * (state.get<Player&>(owner).getcharacter(state).isflipped ? 1:-1);
    }
    else
    {
        return 0;
    }
}

double Hammer::getbackattachpoint_y(Gamestate &state)
{
    if (barrier(state).active)
    {
        return 10;
    }
    else
    {
        return 8;
    }
}

ReinhardtShield& Hammer::barrier(Gamestate &state)
{
    return state.get<ReinhardtShield>(barrierptr);
}

void Hammer::destroy(Gamestate &state)
{
    barrier(state).destroy(state);
    Weapon::destroy(state);
}

void Hammer::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Weapon::interpolate(prev_entity, next_entity, alpha);

    Hammer &prev_e = static_cast<Hammer&>(prev_entity);
    Hammer &next_e = static_cast<Hammer&>(next_entity);

    firestrikeanim.interpolate(prev_e.firestrikeanim, next_e.firestrikeanim, alpha);
    firestrikedelay.interpolate(prev_e.firestrikedelay, next_e.firestrikedelay, alpha);
    firestrikecooldown.interpolate(prev_e.firestrikecooldown, next_e.firestrikecooldown, alpha);
    firinganim.interpolate(prev_e.firinganim, next_e.firinganim, alpha);
    firingdelay1.interpolate(prev_e.firingdelay1, next_e.firingdelay1, alpha);
    firingdelay2.interpolate(prev_e.firingdelay2, next_e.firingdelay2, alpha);
}