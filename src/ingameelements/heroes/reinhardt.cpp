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
#include <SFML/Graphics/RectangleShape.hpp>

void Reinhardt::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Character::init(id_, state, owner_);

    chargeanim.init(herofolder()+"charge/", std::bind(&Reinhardt::endcharge, this, std::placeholders::_1), false);
    preparechargeanim.init(herofolder()+"preparecharge/", std::bind(&Reinhardt::begincharge, this), false);
    endchargeanim.init(herofolder()+"endcharge/", false);
    earthshatteranim.init(herofolder()+"ult/", false);
    earthshatterdelay.init(earthshatteranim.timer.duration * 0.6,
                           std::bind(&Reinhardt::createearthshatter, this, std::placeholders::_1), false);
    shieldrunanim.init(herofolder()+"shieldrun/", true);
    chargecooldown.init(10, false);
}

void Reinhardt::render(Renderer &renderer, Gamestate &state)
{
    Character::render(renderer, state);

    Hammer &hammer = state.get<Hammer&>(weapon);

    // Render weapon back first
    hammer.renderbehind(renderer, state);

    std::string spritepath = currenttorsosprite(state, false);
    sf::Sprite sprite;
    sprite.setPosition(x, y);
    if (isflipped)
    {
        sprite.setScale(-1, 1);
    }

    if (spritepath != NULL_SPRITE)
    {
        renderer.spriteloader.loadsprite(spritepath, sprite);
        renderer.midground.draw(sprite);
        if (state.get<Player>(renderer.myself).team != team)
        {
            // Draw enemy outline too
            renderer.spriteloader.loadspriteoutline(spritepath, sprite);
            sprite.setColor(COLOR_ENEMY_OUTLINE);
            renderer.midground.draw(sprite);
            sprite.setColor(sf::Color::White);
        }
    }

    spritepath = currentsprite(state, false);
    renderer.spriteloader.loadsprite(spritepath, sprite);

    renderer.midground.draw(sprite);
    if (state.get<Player>(renderer.myself).team != team)
    {
        // Draw enemy outline too
        renderer.spriteloader.loadspriteoutline(spritepath, sprite);
        sprite.setColor(COLOR_ENEMY_OUTLINE);
        renderer.midground.draw(sprite);
        sprite.setColor(sf::Color::White);
    }

    hammer.render(renderer, state);

    // Shield health
    double totalwidth = 60;
    int height = 2;
    double left_x = x - totalwidth / 2.0;
    double center_y = sprite.getGlobalBounds().top;

    double hppercent = hammer.barrier(state).hp / hammer.barrier(state).SHIELD_MAX_HP;
    sf::RectangleShape rect;
    // Existing shield hp
    rect.setPosition(left_x, center_y - height);
    rect.setSize(sf::Vector2f(hppercent*totalwidth, height));
    rect.setFillColor(COLOR_SHIELD);
    renderer.surfaceground.draw(rect);
    // Missing shield hp
    rect.setPosition(left_x+ hppercent*totalwidth, center_y - height);
    rect.setSize(sf::Vector2f((1 - hppercent)*totalwidth, height));
    rect.setFillColor(sf::Color(COLOR_SHIELD.r, COLOR_SHIELD.g, COLOR_SHIELD.b, 51));
    renderer.surfaceground.draw(rect);
}

void Reinhardt::beginstep(Gamestate &state, double frametime)
{
    Character::beginstep(state, frametime);

    if (chargeanim.active())
    {
        if (isflipped)
        {
            hspeed = -CHARGE_SPEED;
        }
        else
        {
            hspeed = CHARGE_SPEED;
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
                                            character.damage(state, CHARGE_BUMP_DAMAGE, owner, REINHARDT_CHARGE);
                                            already_bumped_characters.push_back(character.id);
                                            found_collision = true;
                                            if (state.exists(character.id))
                                            {
                                                if (not state.exists(pintarget))
                                                {
                                                    // If they survived and we don't yet have a pinned target, pin them
                                                    character.pinanim.reset();
                                                    pintarget = character.id;
                                                    state.engine.sendbuffer.write<uint8_t>(CHARACTER_PINNED);
                                                    state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
                                                    state.engine.sendbuffer.write<uint8_t>(state.findplayerid(player.id));
                                                }
                                                else
                                                {
                                                    // We already have a pinned target, launch them in the air instead
                                                    double dx = character.x
                                                                - (x + CHARGE_BUMP_XOFFSET * (isflipped?-1:1));
                                                    double dy = character.y - (y + CHARGE_BUMP_YOFFSET);
                                                    if (dx == 0 and dy == 0)
                                                    {
                                                        // If we collided straight with the fist, just get thrown forward
                                                        dx = 1;
                                                        dy = 0;
                                                    }
                                                    double length = std::hypot(dx, dy);
                                                    dx /= length;
                                                    dy /= length;

                                                    character.hspeed = dx * CHARGE_BUMP_STRENGTH;
                                                    character.vspeed = dy * CHARGE_BUMP_STRENGTH;
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
        }

        if (xblockedsmooth.active)
        {
            if (state.exists(pintarget))
            {
                Character &target = state.get<Character&>(pintarget);
                target.damage(state, CHARGE_PIN_DAMAGE, owner, REINHARDT_CHARGE);
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
        target.isflipped = not isflipped;
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
        Character &target = state.get<Character&>(pintarget);
        target.pinanim.active(false);
        while (state.currentmap->collides(target.getcollisionrect(state)))
        {
            target.x -= isflipped ? -1 : 1;
        }
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
    endcharge(state);
    endchargeanim.active(false);
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
        return state.engine.maskloader.get_json_rect(herofolder()+"crouch/").offset(x, y);
    }
    return getstandingcollisionrect(state);
}

Rect Reinhardt::getstandingcollisionrect(Gamestate &state)
{
    return state.engine.maskloader.get_json_rect(herofolder()).offset(x, y);
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
                sf::Image &mask = state.engine.maskloader.loadmask(currenttorsosprite(state, true));
                return mask.getPixel(testx-self.x, testy-self.y).a != 0;
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
