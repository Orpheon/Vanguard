#include "ingameelements/heroes/reinhardt.h"
#include "ingameelements/projectiles/earthshatter.h"
#include "datastructures.h"
#include "spriteloader.h"
#include "animation.h"
#include "gamestate.h"
#include "engine.h"
#include "colorpalette.h"

#include <memory>
#include <cmath>
#include <algorithm>
#include <allegro5/allegro_primitives.h>

void Reinhardt::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Character::init(id_, state, owner_);

    chargeanim.init(herofolder()+"charge/", std::bind(&Reinhardt::endcharge, this, std::placeholders::_1));
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
    chargecooldown.init(10);
    chargecooldown.active = false;
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
    ALLEGRO_BITMAP *outline;
    ALLEGRO_COLOR outlinecolor;

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

    mainsprite = currentsprite(state, false);
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

    state.get<Weapon>(weapon).render(renderer, state);

    // Shield health
    al_set_target_bitmap(renderer.surfaceground);
    mainsprite = currentsprite(state, false);
    double totalwidth = renderer.zoom * 60;
    int height = renderer.zoom * 2;
    double center_x = renderer.zoom * (x - renderer.cam_x);
    double left_x = center_x - totalwidth / 2.0;
    double center_y = renderer.zoom * (y - renderer.spriteloader.get_spriteoffset_y(mainsprite) - renderer.cam_y);
    Hammer &w = state.get<Hammer&>(weapon);
    double hppercent = w.barrier(state).hp / w.barrier(state).SHIELD_MAX_HP;
    al_draw_filled_rectangle(left_x, center_y - height, left_x + hppercent*totalwidth, center_y,
                             ColorPalette::premul(Color::SHIELD, 255));
    al_draw_filled_rectangle(left_x + hppercent*totalwidth, center_y - height, left_x + totalwidth, center_y,
                             ColorPalette::premul(Color::SHIELD, 51));
}

void Reinhardt::beginstep(Gamestate &state, double frametime)
{
    Character::beginstep(state, frametime);

    if (chargeanim.active())
    {
        if (isflipped)
        {
            hspeed = -500;
        }
        else
        {
            hspeed = 500;
        }

        double throwawayvar1, throwawayvar2;
        double maxdist_self = maxdamageabledist(state, &throwawayvar1, &throwawayvar2);
        if (state.engine.isserver)
        {
            for (auto& playerptr : state.playerlist)
            {
                Player &player = state.get<Player&>(playerptr);
                if (state.exists(player.character))
                {
                    Character &character = player.getcharacter(state);
                    if (character.damageableby(team))
                    {
                        double maxdist_other = character.maxdamageabledist(state, &throwawayvar1, &throwawayvar2);
                        if (std::hypot(x - character.x, y - character.y) < maxdist_other + maxdist_self)
                        {
                            if (std::count(already_bumped_characters.begin(), already_bumped_characters.end(),
                                           character.id) == 0)
                            {
                                Rect hitbox = getcollisionrect(state);
                                bool found_collision = false;
                                for (int i=0; i<hitbox.w and not found_collision; ++i)
                                {
                                    for (int j=0; j<hitbox.h and not found_collision; ++j)
                                    {
                                        if (character.collides(state, hitbox.x+i, hitbox.y+j))
                                        {
                                            character.damage(state, CHARGE_BUMP_DAMAGE);
                                            already_bumped_characters.push_back(character.id);
                                            found_collision = true;
                                            if (state.exists(character.id) and not state.exists(pintarget))
                                            {
                                                // If they survived and we don't yet have a pinned target, pin them
                                                character.pinanim.reset();
                                                pintarget = character.id;
                                                state.engine.sendbuffer.write<uint8_t>(CHARACTER_PINNED);
                                                state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
                                                state.engine.sendbuffer.write<uint8_t>(state.findplayerid(player.id));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (xblockedsmooth.active)
        {
            if (state.exists(pintarget))
            {
                Character &target = state.get<Character&>(pintarget);
                target.damage(state, CHARGE_PIN_DAMAGE);
                while (state.currentmap->collides(target.getcollisionrect(state)))
                {
                    target.x -= isflipped ? -1 : 1;
                }
            }
            endcharge(state);
        }
    }
    chargecooldown.update(state, frametime);
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
        // Charging
        if (heldkeys.ABILITY_1 and state.engine.isserver and not chargecooldown.active)
        {
            useability1(state);
            state.engine.sendbuffer.write<uint8_t>(ABILITY1_USED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }
        if (heldkeys.ABILITY_2 and state.engine.isserver and not state.get<Hammer&>(weapon).firestrikecooldown.active)
        {
            useability2(state);
            state.engine.sendbuffer.write<uint8_t>(ABILITY2_USED);
            state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
        }
    }
}

void Reinhardt::endstep(Gamestate &state, double frametime)
{
    Character::endstep(state, frametime);

    if (chargeanim.active() and state.exists(pintarget))
    {
        Character &target = state.get<Character&>(pintarget);
        target.x = x + 10 * (isflipped ? -1:1);
        target.y = y;
        target.hspeed = hspeed;
        target.vspeed = vspeed;
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
    chargecooldown.interpolate(p.chargecooldown, n.chargecooldown, alpha);
}

bool Reinhardt::cangetinput(Gamestate &state)
{
    return  not chargeanim.active() and not preparechargeanim.active() and not endchargeanim.active()
        and not earthshatteranim.active() and Character::cangetinput(state);
}

bool Reinhardt::canuseweapons(Gamestate &state)
{
    Hammer &hammer = state.get<Hammer&>(weapon);
    return Character::canuseweapons(state) and not hammer.firestrikeanim.active();
}

bool Reinhardt::canuseabilities(Gamestate &state)
{
    Hammer &hammer = state.get<Hammer&>(weapon);
    return Character::canuseabilities(state) and not hammer.firestrikeanim.active() and not hammer.firinganim.active();
}

bool Reinhardt::weaponvisible(Gamestate &state)
{
    return Character::weaponvisible(state) and not stunanim.active() and not preparechargeanim.active()
           and not chargeanim.active() and not endchargeanim.active() and not earthshatteranim.active();
}

void Reinhardt::useability1(Gamestate &state)
{
    preparechargeanim.reset();
}

void Reinhardt::useability2(Gamestate &state)
{
    state.get<Hammer&>(weapon).firestrikeanim.reset();
    state.get<Hammer&>(weapon).firestrikedelay.reset();
}

void Reinhardt::useultimate(Gamestate &state)
{
    earthshatteranim.reset();
    earthshatterdelay.reset();
    Player &ownerplayer = state.get<Player>(owner);
    ownerplayer.ultcharge.reset();
}

void Reinhardt::begincharge()
{
    chargeanim.reset();
    already_bumped_characters.clear();
}

void Reinhardt::endcharge(Gamestate &state)
{
    if (state.exists(pintarget))
    {
        state.get<Character&>(pintarget).pinanim.active(false);
    }
    chargeanim.active(false);
    endchargeanim.reset();
    chargecooldown.reset();
    pintarget = 0;
}

void Reinhardt::createearthshatter(Gamestate &state)
{
    int direction = isflipped ? -1 : 1;
    double spawnx = x + direction * 55;
    Rect standingrect = getstandingcollisionrect(state);
    for (double spawny = y - standingrect.h / 2.0; spawny < y + standingrect.h*2; ++spawny)
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
    if (state.exists(weapon))
    {
        Hammer &w = state.get<Hammer&>(weapon);
        w.firinganim.active(false);
        w.firestrikeanim.active(false);
        w.barrier(state).active = false;
    }
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
        std::string torsosprite = currenttorsosprite(state, true);
        if (torsosprite != "NULL_SPRITE")
        {
            Rect self = state.engine.maskloader.get_rect(currenttorsosprite(state, true)).offset(x, y);

            if (testx > self.x and testx < self.x+self.w and testy > self.y and testy < self.y+self.h)
            {
                // We're close enough that an actual collision might happen
                // Check the sprite
                ALLEGRO_BITMAP *selfsprite = state.engine.maskloader.requestsprite(currenttorsosprite(state, true));
                return al_get_pixel(selfsprite, testx-self.x, testy-self.y).a != 0;
            }
        }
        return false;
    }
}

std::string Reinhardt::currentsprite(Gamestate &state, bool mask)
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
    Hammer &hammer = state.get<Hammer>(weapon);
    if (hammer.barrier(state).active)
    {
        if (std::fabs(hspeed) < 11.0 and not heldkeys.LEFT and not heldkeys.RIGHT)
        {
            return herofolder()+"shield/1";
        }
        return shieldrunanim.getframepath();
    }
    if (std::fabs(hspeed) < 11.0 and not heldkeys.LEFT and not heldkeys.RIGHT)
    {
        return herofolder()+"idle/1";
    }
    return runanim.getframepath();
}

std::string Reinhardt::currenttorsosprite(Gamestate &state, bool mask)
{
    if (pinanim.active() or stunanim.active() or earthshatteranim.active() or preparechargeanim.active() or
        chargeanim.active() or endchargeanim.active() or not Character::weaponvisible(state))
    {
        return NULL_SPRITE;
    }
    Hammer &hammer = state.get<Hammer&>(weapon);
    if (hammer.firinganim.active())
    {
        return NULL_SPRITE;
    }
    if (hammer.firestrikeanim.active())
    {
        return herofolder()+"firestriketorso/"+std::to_string(hammer.firestrikeanim.getframe());
    }
    if (crouchanim.active())
    {
        return herofolder()+"crouchtorso/1";
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
    if (hammer.barrier(state).active)
    {
        return NULL_SPRITE;
    }
    if (std::fabs(hspeed) < 11.0 and not heldkeys.LEFT and not heldkeys.RIGHT)
    {
        return herofolder()+"idletorso/1";
    }
    return herofolder()+"runtorso/"+std::to_string(runanim.getframe());
}
