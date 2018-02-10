
#include <gamestate.h>
#include <engine.h>
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
    std::string mainsprite;
    Lucio &c = state.get<Lucio&>(state.get<Player>(owner).character);
    std::string charactersprite = c.currentsprite(state, false);
    if (c.wallriding.active and c.xblockedsmooth.active)
    {
        mainsprite = herofolder() + "wallridebackarm/1";
    }
    else if (c.ampitupbackarm.active())
    {
        mainsprite = c.ampitupbackarm.getframepath();
    }
    else if (c.crossfadeheal.active())
    {
        mainsprite = c.crossfadeheal.getframepath();
    }
    else if (c.crossfadespeed.active())
    {
        mainsprite = c.crossfadespeed.getframepath();
    }
    // FIXME: Someday I'm going to curse myself for this. The arm needs to know whether we're currently running or idling
    else if (charactersprite.find("/run/") != std::string::npos)
    {
        mainsprite = herofolder() + "runbackarm/" + std::to_string(c.runanim.getframe());
    }
    else
    {
        mainsprite = herofolder()+"backarm/1";
    }

    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
    double rel_x = (x - renderer.cam_x)*renderer.zoom;
    double rel_y = (y - renderer.cam_y)*renderer.zoom;
    double attachpt_x = getbackattachpoint_x(state)*renderer.zoom;
    double attachpt_y = getbackattachpoint_y(state)*renderer.zoom;

    ALLEGRO_BITMAP *outline = renderer.spriteloader.requestspriteoutline(mainsprite);
    ALLEGRO_COLOR outlinecolor = al_map_rgb(225, 17, 17);

    al_set_target_bitmap(renderer.midground);
    if (c.weaponvisible(state))
    {
        if (c.isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x-attachpt_x, rel_y-attachpt_y,
                                          -1, 1, 0, 0);
            if (state.get<Player>(renderer.myself).team != team)
            {
                // Draw enemy outline
                al_draw_tinted_scaled_rotated_bitmap(outline, outlinecolor, attachpt_x+spriteoffset_x,
                                                     attachpt_y+spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
            }
        }
        else
        {
            al_draw_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x-attachpt_x, rel_y-attachpt_y, 0, 0);
            if (state.get<Player>(renderer.myself).team != team)
            {
                // Draw enemy outline
                al_draw_tinted_rotated_bitmap(outline, outlinecolor, attachpt_x+spriteoffset_x,
                                              attachpt_y+spriteoffset_y, rel_x, rel_y, 0, 0);
            }
        }
    }
}

void Sonicamp::render(Renderer &renderer, Gamestate &state)
{
    std::string mainsprite;
    double dir = aimdirection;
    Lucio &c = state.get<Lucio>(state.get<Player&>(owner).character);
    if (soundwave.active())
    {
        mainsprite = soundwave.getframepath();
    }
    else if (firingend.active())
    {
        mainsprite = firingend.getframepath();
    }
    else if (firinganim.active())
    {
        mainsprite = firinganim.getframepath();
    }
    else if (reloadanim.active())
    {
        mainsprite = reloadanim.getframepath();
        dir = 3.1415*c.isflipped;
    }
    else
    {
        mainsprite = c.herofolder()+"frontarm/1";
    }
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
    double rel_x = (x - renderer.cam_x)*renderer.zoom;
    double rel_y = (y - renderer.cam_y)*renderer.zoom;
    double attachpt_x = getattachpoint_x(state)*renderer.zoom;
    double attachpt_y = getattachpoint_y(state)*renderer.zoom;

    al_set_target_bitmap(renderer.midground);
    if (c.weaponvisible(state))
    {
        if (c.isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, 1, -1, dir, 0);
        }
        else
        {
            al_draw_rotated_bitmap(sprite, attachpt_x+spriteoffset_x, attachpt_y+spriteoffset_y, rel_x, rel_y, dir, 0);
        }
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
            double dist = std::hypot(x - character.x, y - character.y);
            if (dist <= SOUNDWAVE_RANGE)
            {
                double collisionx, collisiony;
                if (state.collidelinetarget(x, y, character, team, PENETRATE_CHARACTER, &collisionx, &collisiony).id
                    == character.id)
                {
                    double dist = std::hypot(x - collisionx, y - collisiony);
                    double dx = collisionx - x;
                    double dy = collisiony - y;

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
    return 0 * (state.get<Player&>(owner).getcharacter(state).isflipped ? -1:1);
}

double Sonicamp::getbackattachpoint_y(Gamestate &state)
{
    return 0;
}