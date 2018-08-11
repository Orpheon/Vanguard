
#include <gamestate.h>
#include <engine.h>
#include <colorpalette.h>
#include "ingameelements/weapons/sonicamp.h"
#include "ingameelements/heroes/lucio.h"
#include "ingameelements/projectiles/sonicproj.h"

void Sonicamp::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Clipweapon::init(id_, state, owner_);

    soundwavecooldown.init(4, false);
    soundwave.init(herofolder()+"soundwave/", false);
    postsoundwavedelay.init(1.5, false);
    refireloop.init(0.125, std::bind(&Sonicamp::createprojectile, this, std::placeholders::_1), false);
    refiredelay.init(1, false);
    refirecounter = 0;
    firingend.init(herofolder()+"firingend/", false);
}

void Sonicamp::renderbehind(Renderer &renderer, Gamestate &state)
{
    Lucio &lucio = state.get<Lucio&>(state.get<Player>(owner).character);
    if (lucio.weaponvisible(state))
    {
        std::string spritepath;
        if (lucio.ampitupbackarm.active())
        {
            spritepath = lucio.ampitupbackarm.getframepath();
        }
        else if (lucio.crossfadeheal.active())
        {
            spritepath = lucio.crossfadeheal.getframepath();
        }
        else if (lucio.crossfadespeed.active())
        {
            spritepath = lucio.crossfadespeed.getframepath();
        }
            // FIXME: Someday I'm going to curse myself for this. The arm needs to know whether we're currently running or idling
        else if (lucio.currentsprite(state, false).find("/run/") != std::string::npos)
        {
            spritepath = herofolder() + "runbackarm/" + std::to_string(lucio.runanim.getframe());
        }
        else
        {
            spritepath = herofolder()+"backarm/1";
        }
        sf::Sprite sprite;
        renderer.spriteloader.loadsprite(spritepath, sprite);
        sprite.setPosition(x - getbackattachpoint_x(state), y - getbackattachpoint_y(state));
        if (lucio.isflipped)
        {
            sprite.setScale(-1, 1);
        }
        renderer.midground.draw(sprite);
        if (state.get<Player>(renderer.myself).team != team)
        {
            // Draw enemy outline
            sprite.setColor(COLOR_ENEMY_OUTLINE);
            renderer.spriteloader.loadspriteoutline(spritepath, sprite);
            renderer.midground.draw(sprite);
            sprite.setColor(sf::Color::White);
        }
    }
}

void Sonicamp::render(Renderer &renderer, Gamestate &state)
{
    Lucio &lucio = state.get<Lucio>(state.get<Player&>(owner).character);
    if (lucio.weaponvisible(state))
    {
        std::string spritepath;
        double dir = aimdirection;
        if (soundwave.active())
        {
            spritepath = soundwave.getframepath();
        }
        else if (firingend.active())
        {
            spritepath = firingend.getframepath();
        }
        else if (firinganim.active())
        {
            spritepath = firinganim.getframepath();
        }
        else if (reloadanim.active())
        {
            spritepath = reloadanim.getframepath();
            dir = 3.1415*lucio.isflipped;
        }
        else
        {
            spritepath = lucio.herofolder()+"frontarm/1";
        }

        sf::Sprite sprite;
        renderer.spriteloader.loadsprite(spritepath, sprite);
        sprite.setPosition(x-getattachpoint_x(state), y-getattachpoint_y(state));
        sprite.setRotation(dir*180.0/3.1415);
        if (lucio.isflipped)
        {
            sprite.setScale(-1, 1);
        }
        renderer.midground.draw(sprite);
    }
}

void Sonicamp::midstep(Gamestate &state, double frametime)
{
    Clipweapon::midstep(state, frametime);

    soundwavecooldown.update(state, frametime);
    soundwave.update(state, frametime);
    postsoundwavedelay.update(state, frametime);
    refireloop.update(state, frametime);
    refiredelay.update(state, frametime);
    firingend.update(state, frametime);
}

void Sonicamp::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Clipweapon::interpolate(prev_entity, next_entity, alpha);

    Sonicamp &p = static_cast<Sonicamp&>(prev_entity);
    Sonicamp &n = static_cast<Sonicamp&>(next_entity);

    soundwave.interpolate(p.soundwave, n.soundwave, alpha);
    soundwavecooldown.interpolate(p.soundwavecooldown, n.soundwavecooldown, alpha);
    postsoundwavedelay.interpolate(p.postsoundwavedelay, n.postsoundwavedelay, alpha);
    refireloop.interpolate(p.refireloop, n.refireloop, alpha);
    refiredelay.interpolate(p.refiredelay, n.refiredelay, alpha);
    firingend.interpolate(p.firingend, n.firingend, alpha);
}

void Sonicamp::fireprimary(Gamestate &state)
{
    refirecounter = 0;
    refireloop.reset();
    firinganim.reset();
}

void Sonicamp::createprojectile(Gamestate &state)
{
    if (clip > 0)
    {
        SonicProj &proj = state.get<SonicProj&>(state.make_entity<SonicProj>(state, owner));
        proj.x = x + std::cos(aimdirection) * 14;
        proj.y = y + std::sin(aimdirection) * 14;
        proj.hspeed = proj.SPEED * std::cos(aimdirection);
        proj.vspeed = proj.SPEED * std::sin(aimdirection);

        if (state.currentmap->collideline(x, y, proj.x, proj.y))
        {
            proj.destroy(state);
        }

        ++refirecounter;
        --clip;
    }
    if (clip > 0 and refirecounter < 4)
    {
        if (refirecounter < 3)
        {
            firinganim.reset();
        }
        else
        {
            firingend.reset();
        }
        refireloop.reset_after_eventfunc();
    }
    else
    {
        refirecounter = 0;
        refiredelay.reset();
    }
}

void Sonicamp::firesecondary(Gamestate &state)
{
    for (auto &p : state.playerlist)
    {
        Player &player = state.get<Player>(p);
        if (player.team != team and state.exists(player.character))
        {
            Character &character = player.getcharacter(state);
            Rect characterrect = state.engine.maskloader.get_rect(character.currentsprite(state, true));
            double character_x = characterrect.x + characterrect.w/2.0;
            double character_y = characterrect.y + characterrect.h/2.0;
            double dist = std::hypot(x - character_x, y - character_y);
            if (dist <= SOUNDWAVE_RANGE)
            {
                if (state.collidelineshielded(x, y, character_x, character_y, character, team, PENETRATE_CHARACTER).id
                    == character.id)
                {
                    double dist = std::hypot(x - character_x, y - character_y);
                    double dx = character_x - x;
                    double dy = character_y - y;

                    double dalpha = std::atan2(dy, dx) - aimdirection;
                    while (dalpha > PI)
                    {
                        dalpha -= 2.0*PI;
                    }
                    while (dalpha <= -PI)
                    {
                        dalpha += 2.0*PI;
                    }
                    if (dalpha <= SOUNDWAVE_ANGLEWIDTH)
                    {
                        if (dist != 0)
                        {
                            dx /= dist;
                            dy /= dist;
                        }
                        else
                        {
                            dx = 1;
                            dy = 0;
                        }

                        character.hspeed += dx * SOUNDWAVE_FORCE;
                        character.vspeed += dy * SOUNDWAVE_FORCE;

                        double effectivedamage = character.damage(state, SOUNDWAVE_DAMAGE, owner, LUCIO_SOUNDWAVE);
                        state.get<Player&>(owner).registerdamage(state, effectivedamage);
                    }
                }
            }
        }
    }

    clip = std::max(0, clip-4);

    soundwave.reset();
    soundwavecooldown.reset();
    postsoundwavedelay.reset();
}

void Sonicamp::wantfireprimary(Gamestate &state)
{
    if (clip > 0 and not refireloop.active and not refiredelay.active and not reloadanim.active()
        and not postsoundwavedelay.active and state.engine.isserver)
    {
        fireprimary(state);
        state.engine.sendbuffer.write<uint8_t>(PRIMARY_FIRED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
    }
}

void Sonicamp::wantfiresecondary(Gamestate &state)
{
    if (clip > 0 and not firinganim.active() and not reloadanim.active() and not soundwavecooldown.active
        and state.engine.isserver)
    {
        firesecondary(state);
        state.engine.sendbuffer.write<uint8_t>(SECONDARY_FIRED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
    }
}

double Sonicamp::getbackattachpoint_x(Gamestate &state)
{
    return -7 * (state.get<Player&>(owner).getcharacter(state).isflipped ? -1:1);
}

double Sonicamp::getbackattachpoint_y(Gamestate &state)
{
    return 12;
}