#include "ingameelements/heroes/reinhardt.h"
#include "ingameelements/projectiles/earthshatter.h"
#include "datastructures.h"
#include "spriteloader.h"
#include "animation.h"
#include "gamestate.h"
#include "engine.h"

#include <memory>
#include <cmath>
#include <allegro5/allegro_primitives.h>

void Reinhardt::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Character::init(id_, state, owner_);

    chargeanim.init(herofolder()+"charge/", std::bind(&Reinhardt::endcharge, this));
    chargeanim.active(false);
    preparechargeanim.init(herofolder()+"preparecharge/", std::bind(&Reinhardt::begincharge, this));
    preparechargeanim.active(false);
    endchargeanim.init(herofolder()+"endcharge/");
    endchargeanim.active(false);
    earthshatteranim.init(herofolder()+"ult/");
    earthshatteranim.active(false);
    earthshatterdelay.init(earthshatteranim.timer.duration * 0.6,
                           std::bind(&Reinhardt::createearthshatter, this, std::placeholders::_1));
    earthshatterdelay.active = false;
    shieldrunanim.init(herofolder()+"shieldrun/");
}

void Reinhardt::render(Renderer &renderer, Gamestate &state)
{
    Character::render(renderer, state);
    al_set_target_bitmap(renderer.midground);

    // Render weapon back first
    state.get<Hammer>(weapon).renderbehind(renderer, state);

    std::string mainsprite;
    ALLEGRO_BITMAP *sprite;
    double spriteoffset_x, spriteoffset_y;
    double rel_x, rel_y;

    mainsprite = currentsprite(state, false);
    sprite = renderer.spriteloader.requestsprite(mainsprite);
    spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
    rel_x = (x-renderer.cam_x)*renderer.zoom;
    rel_y = (y-renderer.cam_y)*renderer.zoom;

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

    mainsprite = currenttorsosprite(state, false);
    if (mainsprite != NULL_SPRITE)
    {
        sprite = renderer.spriteloader.requestsprite(mainsprite);
        spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
        spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
        rel_x = (x-renderer.cam_x)*renderer.zoom;
        rel_y = (y-renderer.cam_y)*renderer.zoom;

        outline = renderer.spriteloader.requestspriteoutline(mainsprite);
        outlinecolor = al_map_rgb(225, 17, 17);

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
    }

    state.get<Weapon>(weapon).render(renderer, state);
}

void Reinhardt::drawhud(Renderer &renderer, Gamestate &state)
{
    Character::drawhud(renderer, state);
}

void Reinhardt::midstep(Gamestate &state, double frametime)
{
    Character::midstep(state, frametime);

    if (chargeanim.active())
    {
        if (xblocked)
        {
            endcharge();
        }
        if (isflipped)
        {
            hspeed = -500;
        }
        else
        {
            hspeed = 500;
        }
    }
    chargeanim.update(state, frametime);
    preparechargeanim.update(state, frametime);
    endchargeanim.update(state, frametime);
    if (onground(state) or earthshatteranim.getframe() != 9)
    {
        earthshatteranim.update(state, frametime);
        earthshatterdelay.update(state, frametime);
    }
    if (onground(state))
    {
        if (isflipped)
        {
            shieldrunanim.update(state, -hspeed*frametime);
        }
        else
        {
            shieldrunanim.update(state, hspeed*frametime);
        }
    }

    if (canuseabilities(state))
    {
        if (heldkeys.ABILITY_1 and state.engine.isserver)
        {
            useability1(state);
            state.engine.sendbuffer.write<uint8_t>(ABILITY1_USED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }
        if (heldkeys.ABILITY_2 and state.engine.isserver)
        {
            useability2(state);
            state.engine.sendbuffer.write<uint8_t>(ABILITY2_USED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }
    }
}

void Reinhardt::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    Character::interpolate(prev_entity, next_entity, alpha);

    Reinhardt &p = static_cast<Reinhardt&>(prev_entity);
    Reinhardt &n = static_cast<Reinhardt&>(next_entity);

    preparechargeanim.interpolate(p.preparechargeanim, n.preparechargeanim, alpha);
    chargeanim.interpolate(p.chargeanim, n.chargeanim, alpha);
    endchargeanim.interpolate(p.endchargeanim, n.endchargeanim, alpha);
    earthshatteranim.interpolate(p.earthshatteranim, n.earthshatteranim, alpha);
    earthshatterdelay.interpolate(p.earthshatterdelay, n.earthshatterdelay, alpha);
    shieldrunanim.interpolate(p.shieldrunanim, n.shieldrunanim, alpha);
}

bool Reinhardt::cangetinput(Gamestate &state)
{
    return  not chargeanim.active() and not preparechargeanim.active() and not endchargeanim.active()
        and not earthshatteranim.active() and Character::cangetinput(state);
}

bool Reinhardt::weaponvisible(Gamestate &state)
{
    return  not stunanim.active() and not preparechargeanim.active() and not chargeanim.active()
        and not endchargeanim.active() and not earthshatteranim.active();
}

void Reinhardt::useability1(Gamestate &state)
{
    preparechargeanim.reset();
}

void Reinhardt::useability2(Gamestate &state)
{

}

void Reinhardt::useultimate(Gamestate &state)
{
    earthshatteranim.reset();
    earthshatterdelay.reset();
    Player &ownerplayer = state.get<Player>(owner);
    ownerplayer.ultcharge.reset();
}

void Reinhardt::createearthshatter(Gamestate &state)
{
    int direction = isflipped ? -1 : 1;
    double spawnx = x + direction * 40;
    for (double spawny = y; spawny < y + getstandingcollisionrect(state).h*2; ++spawny)
    {
        if (not state.currentmap->testpixel(spawnx, spawny) and state.currentmap->testpixel(spawnx, spawny+1))
        {
            Earthshatter &shockwave = state.get<Earthshatter>(state.make_entity<Earthshatter>(state, owner));
            shockwave.x = spawnx;
            shockwave.y = spawny;
            break;
        }
    }
}

void Reinhardt::interrupt(Gamestate &state)
{
    chargeanim.active(false);
    earthshatteranim.active(false);
    earthshatterdelay.active = false;
}

Rect Reinhardt::getcollisionrect(Gamestate &state)
{
    if (crouchanim.active())
    {
        return state.engine.maskloader.get_rect_from_json(herofolder()+"crouch/").offset(x, y);
    }
    return getstandingcollisionrect(state);
}

Rect Reinhardt::getstandingcollisionrect(Gamestate &state)
{
    return state.engine.maskloader.get_rect_from_json(herofolder()).offset(x, y);
}

bool Reinhardt::collides(Gamestate &state, double testx, double testy)
{
    if (Character::collides(state, testx, testy))
    {
        return true;
    }
    else
    {
        Rect self = state.engine.maskloader.get_rect(currenttorsosprite(state, true)).offset(x, y);

        if (testx > self.x and testx < self.x+self.w and testy > self.y and testy < self.y+self.h)
        {
            // We're close enough that an actual collision might happen
            // Check the sprite
            ALLEGRO_BITMAP *selfsprite = state.engine.maskloader.requestsprite(currenttorsosprite(state, true));
            return al_get_pixel(selfsprite, testx-self.x, testy-self.y).a != 0;
        }
        else
        {
            return false;
        }
    }
}

std::string Reinhardt::currentsprite(Gamestate &state, bool mask)
{
    if (stunanim.active())
    {
        return stunanim.getframepath();
    }
    if (earthshatteranim.active())
    {
        return earthshatteranim.getframepath();
    }
    if (preparechargeanim.active())
    {
        return preparechargeanim.getframepath();
    }
    if (chargeanim.active())
    {
        return chargeanim.getframepath();
    }
    if (endchargeanim.active())
    {
        return endchargeanim.getframepath();
    }
    if (crouchanim.active())
    {
        return crouchanim.getframepath();
    }
    /*if (not ongroundsmooth.active)
    {
        if (vspeed > 100)
        {
            return herofolder()+"falling/1";
        }
        else
        {
            return herofolder()+"jump/1";
        }
    }*/
    if (std::fabs(hspeed) < 11.0 and not heldkeys.LEFT and not heldkeys.RIGHT)
    {
        return herofolder()+"idle/1";
    }
    Hammer &hammer = state.get<Hammer>(weapon);
    if (hammer.barrier(state).active)
    {
        return shieldrunanim.getframepath();
    }
    return runanim.getframepath();
}

std::string Reinhardt::currenttorsosprite(Gamestate &state, bool mask)
{
    if (stunanim.active() or earthshatteranim.active() or preparechargeanim.active() or chargeanim.active() or endchargeanim.active())
    {
        return NULL_SPRITE;
    }
    if (crouchanim.active())
    {
        return herofolder()+"crouchwalktorso/"+std::to_string(crouchanim.getframe());
    }
    /*if (not ongroundsmooth.active)
    {
        if (vspeed > 100)
        {
            return herofolder()+"falling/1";
        }
        else
        {
            return herofolder()+"jump/1";
        }
    }*/
    if (std::fabs(hspeed) < 11.0 and not heldkeys.LEFT and not heldkeys.RIGHT)
    {
        return herofolder()+"idletorso/1";
    }
    Hammer &hammer = state.get<Hammer>(weapon);
    if (hammer.barrier(state).active)
    {
        return NULL_SPRITE;
    }
    return herofolder()+"runtorso/"+std::to_string(runanim.getframe());
}
