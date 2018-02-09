
#include "engine.h"
#include "ingameelements/heroes/lucio.h"

#include "allegro5/allegro_primitives.h"

void Lucio::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Character::init(id_, state, owner_);

    wallriding.init(0.4);
    wallriding.active = false;
    ampitup.init(3);
    ampitup.active = false;
    ampitupcooldown.init(12);
    ampitupcooldown.active = false;
    ampitupbackarm.init(herofolder()+"ampitupbackarm/");
    ampitupbackarm.active(false);
    ampitupstanding.init(herofolder()+"ampitupstanding/");
    ampitupstanding.active(false);
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

void Lucio::midstep(Gamestate &state, double frametime)
{
    Character::midstep(state, frametime);

    if (xblockedsmooth.active and not onground(state))
    {
        // We're wallriding
        vspeed = std::min(vspeed, 0.0);
        wallriding.reset();
    }
    wallriding.update(state, frametime);
}

void Lucio::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Character::interpolate(prev_entity, next_entity, alpha);

    Lucio &p = static_cast<Lucio&>(prev_entity);
    Lucio &n = static_cast<Lucio&>(next_entity);

    wallriding.interpolate(p.wallriding, n.wallriding, alpha);
    ampitup.interpolate(p.ampitup, n.ampitup, alpha);
    ampitupcooldown.interpolate(p.ampitupcooldown, n.ampitupcooldown, alpha);
    ampitupbackarm.interpolate(p.ampitupbackarm, n.ampitupbackarm, alpha);
    ampitupstanding.interpolate(p.ampitupstanding, n.ampitupstanding, alpha);
}

void Lucio::useability1(Gamestate &state)
{
    Global::logging().print(__FILE__, __LINE__, "Crossfade used.");
}

void Lucio::useability2(Gamestate &state)
{
    Global::logging().print(__FILE__, __LINE__, "Amp it up used.");
}

void Lucio::useultimate(Gamestate &state)
{
    Global::logging().print(__FILE__, __LINE__, "Soundbarrier used.");
}

void Lucio::interrupt(Gamestate &state)
{
    ;
}

bool Lucio::canjump(Gamestate &state)
{
    return (onground(state) or wallriding.active) and not jumpcooldown.active;
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
    if (crouchanim.active())
    {
        return crouchanim.getframepath();
    }
    if (not ongroundsmooth.active)
    {
        return herofolder()+"jump/1";
    }
    if (std::fabs(hspeed) < 11.0 and not heldkeys.LEFT and not heldkeys.RIGHT)
    {
        return herofolder()+"idle/1";
    }
    return runanim.getframepath();
}