
#include "engine.h"
#include "ingameelements/heroes/lucio.h"

#include "allegro5/allegro_primitives.h"

void Lucio::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Character::init(id_, state, owner_);

    wallriding.init(0.4, false);
    wallridejumpcooldown.init(10, false);
    ampitup.init(3, false);
    ampitupcooldown.init(2, false);
    ampitupbackarm.init(herofolder()+"ampitupbackarm/", false);
    ampitupstanding.init(herofolder()+"ampitupstanding/", false);
    crossfadeheal.init(herofolder()+"crossfadehealbackarm/", false);
    crossfadespeed.init(herofolder()+"crossfadespeedbackarm/", false);

    currentaura = SPEEDAURA;
}

void Lucio::render(Renderer &renderer, Gamestate &state)
{
    Character::render(renderer, state);
    al_set_target_bitmap(renderer.midground);

    state.get<Sonicamp&>(weapon).renderbehind(renderer, state);

    std::string mainsprite;
    ALLEGRO_BITMAP *sprite;
    double spriteoffset_x, spriteoffset_y;
    double rel_x, rel_y;
    rel_x = (x-renderer.cam_x)*renderer.zoom;
    rel_y = (y-renderer.cam_y)*renderer.zoom;

    mainsprite = currentsprite(state, false);
    sprite = renderer.spriteloader.requestsprite(mainsprite);
    spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;

    ALLEGRO_BITMAP *outline = renderer.spriteloader.requestspriteoutline(mainsprite);
    ALLEGRO_COLOR outlinecolor = al_map_rgb(225, 17, 17);

    if (isflipped)
    {
        // Flip horizontally
        al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
        if (state.get<Player>(renderer.myself).team != team)
        {
            // Draw enemy outline
            al_draw_tinted_scaled_rotated_bitmap(outline, outlinecolor, spriteoffset_x, spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
        }
    }
    else
    {
        al_draw_bitmap(sprite, rel_x-spriteoffset_x, rel_y-spriteoffset_y, 0);
        if (state.get<Player>(renderer.myself).team != team)
        {
            // Draw enemy outline
            al_draw_tinted_bitmap(outline, outlinecolor, rel_x-spriteoffset_x, rel_y-spriteoffset_y, 0);
        }
    }

    state.get<Weapon>(weapon).render(renderer, state);
}

void Lucio::beginstep(Gamestate &state, double frametime)
{
    Character::beginstep(state, frametime);

    if (ampitupstanding.active() and (heldkeys.LEFT or heldkeys.RIGHT or not onground(state)))
    {
        ampitupstanding.active(false);
    }

    ampitup.update(state, frametime);
    ampitupcooldown.update(state, frametime);
    ampitupbackarm.update(state, frametime);
    ampitupstanding.update(state, frametime);
    crossfadeheal.update(state, frametime);
    crossfadespeed.update(state, frametime);
    wallridejumpcooldown.update(state, hspeed * frametime);

    if (canuseabilities(state))
    {
        if (heldkeys.ABILITY_1 and not crossfadeheal.active() and not crossfadespeed.active())
        {
            useability1(state);
            state.engine.sendbuffer.write<uint8_t>(ABILITY1_USED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }

        if (heldkeys.ABILITY_2 and not ampitupcooldown.active and state.engine.isserver)
        {
            useability2(state);
            state.engine.sendbuffer.write<uint8_t>(ABILITY2_USED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }
    }
}

void Lucio::midstep(Gamestate &state, double frametime)
{
    Character::midstep(state, frametime);

    for (auto &p : state.playerlist)
    {
        Player &player = state.get<Player>(p);
        if (player.team == team and state.exists(player.character))
        {
            Character &character = player.getcharacter(state);
            double dist = std::hypot(x - character.x, y - character.y);
            if (dist <= AURARANGE)
            {
                if (currentaura == HEALAURA)
                {
                    if (ampitup.active)
                    {
                        character.heal(state, 45 * frametime);
                    }
                    else
                    {
                        character.heal(state, 15 * frametime);
                    }
                }
                else
                {
                    if (ampitup.active)
                    {
                        character.speedboost = 1.7;
                    }
                    else
                    {
                        character.speedboost = 1.3;
                    }
                }
            }
        }
    }

    if (xblockedsmooth.active and not onground(state))
    {
        // We're wallriding
        vspeed = std::min(vspeed, 0.0);
        wallriding.reset();
    }
    wallriding.update(state, frametime);
    if (onground(state))
    {
        wallridejumpcooldown.active = false;
    }
}

void Lucio::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Character::interpolate(prev_entity, next_entity, alpha);

    Lucio &p = static_cast<Lucio&>(prev_entity);
    Lucio &n = static_cast<Lucio&>(next_entity);

    wallriding.interpolate(p.wallriding, n.wallriding, alpha);
    wallridejumpcooldown.interpolate(p.wallridejumpcooldown, n.wallridejumpcooldown, alpha);
    ampitup.interpolate(p.ampitup, n.ampitup, alpha);
    ampitupcooldown.interpolate(p.ampitupcooldown, n.ampitupcooldown, alpha);
    ampitupbackarm.interpolate(p.ampitupbackarm, n.ampitupbackarm, alpha);
    ampitupstanding.interpolate(p.ampitupstanding, n.ampitupstanding, alpha);
    crossfadeheal.interpolate(p.crossfadeheal, n.crossfadeheal, alpha);
    crossfadespeed.interpolate(p.crossfadespeed, n.crossfadespeed, alpha);
}

void Lucio::useability1(Gamestate &state)
{
    Global::logging().print(__FILE__, __LINE__, "Crossfade used.");
    if (currentaura == HEALAURA)
    {
        crossfadespeed.reset();
        currentaura = SPEEDAURA;
    }
    else
    {
        crossfadeheal.reset();
        currentaura = HEALAURA;
    }
}

void Lucio::useability2(Gamestate &state)
{
    Global::logging().print(__FILE__, __LINE__, "Amp it up used.");
    ampitup.reset();
    ampitupcooldown.reset();
    if (heldkeys.LEFT or heldkeys.RIGHT or not onground(state))
    {
        ampitupbackarm.reset();
    }
    else
    {
        ampitupstanding.reset();
    }
}

void Lucio::useultimate(Gamestate &state)
{
    Global::logging().print(__FILE__, __LINE__, "Soundbarrier used.");
}

void Lucio::interrupt(Gamestate &state)
{
    ampitupstanding.active(false);
    ampitupbackarm.active(false);
}

bool Lucio::canjump(Gamestate &state)
{
    return (onground(state) or wallriding.active) and not wallridejumpcooldown.active and vspeed > -50;
}

void Lucio::jump(Gamestate &state)
{
    Character::jump(state);
    if (not onground(state))
    {
        wallridejumpcooldown.reset();
    }
}

bool Lucio::weaponvisible(Gamestate &state)
{
    return Character::weaponvisible(state) and not ampitupstanding.active();
}

Rect Lucio::getcollisionrect(Gamestate &state)
{
    if (crouchanim.active())
    {
        return state.engine.maskloader.get_rect_from_json(herofolder()+"crouch/").offset(x, y);
    }
    return getstandingcollisionrect(state);
}

Rect Lucio::getstandingcollisionrect(Gamestate &state)
{
    return state.engine.maskloader.get_rect_from_json(herofolder()).offset(x, y);
}

std::string Lucio::currentsprite(Gamestate &state, bool mask)
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
    if (ampitupstanding.active())
    {
        return ampitupstanding.getframepath();
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