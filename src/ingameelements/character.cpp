#include <vector>
#include <cmath>

#include "ingameelements/character.h"
#include "gamestate.h"
#include "ingameelements/movingentity.h"
#include "ingameelements/player.h"
#include "global_constants.h"
#include "engine.h"
#include "ingameelements/weapon.h"
#include "ingameelements/corpse.h"

Character::Character(uint64_t id_, Gamestate *state, EntityPtr owner_, CharacterChildParameters parameters) : MovingEntity(id_, state),
            owner(owner_), weapon(parameters.weapon), hp(parameters.maxhp), isflipped(false), runanim(parameters.characterfolder+"run/"),
            crouchanim(parameters.characterfolder+"crouchwalk/"), pressed_keys(), held_keys(), lastdirectionpressed(0)
{
    acceleration = 300;
    runpower = parameters.runpower;
    // friction factor per second of null movement; calculated directly from Gang Garrison 2
    // from pygg2
    friction = 0.01510305449388463132584804061124;

    entitytype = CHARACTER;
}

void Character::setinput(Gamestate *state, INPUT_CONTAINER pressed_keys_, INPUT_CONTAINER held_keys_, double mouse_x_, double mouse_y_)
{
    pressed_keys = pressed_keys_;
    held_keys = held_keys_;
    mouse_x = mouse_x_ - x;
    mouse_y = mouse_y_ - y;
    getweapon(state)->setaim(mouse_x, mouse_y);
}

void Character::beginstep(Gamestate *state, double frametime)
{
    getweapon(state)->beginstep(state, frametime);
}

void Character::midstep(Gamestate *state, double frametime)
{
    if (cangetinput(state))
    {
        double maxhspeed;
        if (crouchanim.active())
        {
            maxhspeed = 60.0;
        }
        else
        {
            maxhspeed = 153.0;
        }

        if (pressed_keys.LEFT)
        {
            lastdirectionpressed = LEFT;
        }
        else if (lastdirectionpressed == LEFT and not held_keys.LEFT)
        {
            lastdirectionpressed = RIGHT*held_keys.RIGHT;
        }
        if (pressed_keys.RIGHT)
        {
            lastdirectionpressed = RIGHT;
        }
        else if (lastdirectionpressed == RIGHT and not held_keys.RIGHT)
        {
            lastdirectionpressed = LEFT*held_keys.LEFT;
        }

        if (lastdirectionpressed == LEFT)
        {
            hspeed = std::max(hspeed - acceleration * runpower * frametime, -maxhspeed);
        }
        else if (lastdirectionpressed == RIGHT)
        {
            hspeed = std::min(hspeed + acceleration * runpower * frametime, maxhspeed);
        }

        if (pressed_keys.JUMP)
        {
            if (onground(state))
            {
                vspeed = -240.0;
            }
        }
        if (held_keys.CROUCH)
        {
            if (not crouchanim.active())
            {
                crouchanim.active(true);
                crouchanim.reset();
            }
        }
        else if (crouchanim.active())
        {
            // We're crouched and we'd like to uncrouch
            // Do so only if we have the room
            if (not state->currentmap->collides(getstandingcollisionrect(state)))
            {
                crouchanim.active(false);
            }
        }

        if (held_keys.RELOAD)
        {
            getweapon(state)->reload(state);
        }
        // Shooting
        if (held_keys.PRIMARY_FIRE)
        {
            getweapon(state)->wantfireprimary(state);
        }
        if (held_keys.SECONDARY_FIRE)
        {
            getweapon(state)->wantfiresecondary(state);
        }

        if (isflipped != (mouse_x < 0))
        {
            // Spinjumping (compensate for later gravity)
            vspeed -= 540.0*frametime*3.0/4.0;
            isflipped = (mouse_x < 0);
        }
    }

    // Gravity
    vspeed += 540.0*frametime;

    // apply friction
    hspeed *= std::pow(friction, frametime);

    getweapon(state)->midstep(state, frametime);
}

void Character::endstep(Gamestate *state, double frametime)
{
    MovingEntity::endstep(state, frametime);

    // Collision with wallmask
    if (state->currentmap->collides(getcollisionrect(state)))
    {
        // We collide, do collision handling
        double hs = hspeed*frametime, vs = vspeed*frametime;
        // Buffers for "undone" horizontal/vertical movement
        double xbuffer = 0.0, ybuffer = 0.0;
        int steps = std::ceil(std::fmax(std::abs(hs), std::abs(vs)));
        double xstep = hs/steps, ystep = vs/steps;
        // Pull us out of the wall
        for (int i=0; i<steps; ++i)
        {
            x -= xstep; y -= ystep;
            xbuffer += xstep; ybuffer += ystep;
            if (not state->currentmap->collides(getcollisionrect(state)))
            {
                break;
            }
        }
        // We're at the point where the character touched the wallmask for the first time
        // Now keep moving one unit in either direction until all possible movement is exhausted
        bool xblocked = false, yblocked = false;
        bool xfinished = false, yfinished = false;
        double oldxbuffer = xbuffer, oldybuffer = ybuffer;
        while (not xfinished or not yfinished)
        {
            oldxbuffer = xbuffer; oldybuffer = ybuffer;
            // Try first moving horizontally
            if (not xfinished)
            {
                if (not state->currentmap->collides(getcollisionrect(state).offset(xstep, 0)))
                {
                    x += xstep;
                    xbuffer -= xstep;
                    if (xbuffer == 0.0 or std::signbit(xbuffer) != std::signbit(oldxbuffer))
                    {
                        // We're done for this coordinate
                        xfinished = true;
                    }
                }
                else
                {
                    // There's the possibility we might have walked into a staircase
                    if (not state->currentmap->collides(getcollisionrect(state).offset(xstep, STAIRCASE_STEPSIZE)))
                    {
                        // Indeed we did. Move up
                        x += xstep;
                        y += STAIRCASE_STEPSIZE;
                        xbuffer -= xstep;
                        if (xbuffer == 0.0 or std::signbit(xbuffer) != std::signbit(oldxbuffer))
                        {
                            // We're done for this coordinate
                            xfinished = true;
                        }
                    }
                    else
                    {
                        // No more moving, we're blocked
                        xblocked = true;
                        xfinished = true;
                    }
                }
            }
            // Do the same vertically, but without stair code
            if (not yfinished)
            {
                if (not state->currentmap->collides(getcollisionrect(state).offset(0, ystep)))
                {
                    // We cam move through
                    y += ystep;
                    ybuffer -= ystep;
                    if (ybuffer == 0.0 or std::signbit(ybuffer) != std::signbit(oldybuffer))
                    {
                        // We're done for this coordinate
                        yfinished = true;
                    }
                }
                else
                {
                    // No more moving, we're blocked
                    yblocked = true;
                    yfinished = true;
                }
            }
        }
        // Set hspeed and vspeed to what they should be
        if (xblocked)
        {
            hspeed = 0;
        }
        if (yblocked)
        {
            vspeed = 0;
        }
        if (state->currentmap->collides(getcollisionrect(state)))
        {
            printf("\nError: Still in wallmask after collision handling.");
        }
    } // end collision with wallmask

    // Stick to stairs when going down
    if (std::fabs(vspeed*frametime) <= 3.0 and not std::signbit(vspeed*frametime) and not onground(state))
    {
        Rect r = getcollisionrect(state);
        int stepsize = (-STAIRCASE_STEPSIZE)*std::ceil(std::fabs(hspeed)*frametime/(-STAIRCASE_STEPSIZE));
        if (state->currentmap->collides(Rect(r.x, r.y+r.h+stepsize, r.w, 1)))
        {
            for (int i=0; i<stepsize; ++i)
            {
                ++y;
                if (onground(state))
                {
                    break;
                }
            }
        }
    }

    // Running animation
    if (onground(state))
    {
        if (isflipped)
        {
            runanim.update(state, -hspeed*frametime);
            crouchanim.update(state, -hspeed*frametime);
        }
        else
        {
            runanim.update(state, hspeed*frametime);
            crouchanim.update(state, hspeed*frametime);
        }
    }
    if (hspeed == 0.0)
    {
        bool run=runanim.active(), crouch=crouchanim.active();
        runanim.reset();
        runanim.active(run);
        crouchanim.reset();
        crouchanim.active(crouch);
    }

    getweapon(state)->endstep(state, frametime);
}

bool Character::onground(Gamestate *state)
{
    Rect r = getcollisionrect(state);
    return state->currentmap->collides(Rect(r.x, r.y+r.h, r.w, 1));
}

void Character::interpolate(Entity *prev_entity, Entity *next_entity, double alpha)
{
    MovingEntity::interpolate(prev_entity, next_entity, alpha);

    Character *prev_e = static_cast<Character*>(prev_entity);
    Character *next_e = static_cast<Character*>(next_entity);

    if (alpha < 0.5)
    {
        held_keys = prev_e->held_keys;
        pressed_keys = prev_e->pressed_keys;
        crouchanim.active(prev_e->crouchanim.active());
    }
    else
    {
        held_keys = next_e->held_keys;
        pressed_keys = next_e->pressed_keys;
        crouchanim.active(prev_e->crouchanim.active());
    }
    mouse_x = prev_e->mouse_x + alpha*(next_e->mouse_x - prev_e->mouse_x);
    mouse_y = prev_e->mouse_y + alpha*(next_e->mouse_y - prev_e->mouse_y);
    runanim.interpolate(&(prev_e->runanim), &(next_e->runanim), alpha);
    hp.normal = prev_e->hp.normal + alpha*(next_e->hp.normal - prev_e->hp.normal);
    hp.armor = prev_e->hp.armor + alpha*(next_e->hp.armor - prev_e->hp.armor);
    hp.shields = prev_e->hp.shields + alpha*(next_e->hp.shields - prev_e->hp.shields);
}

void Character::serialize(Gamestate *state, WriteBuffer *buffer, bool fullupdate)
{
    MovingEntity::serialize(state, buffer, fullupdate);

    buffer->write<uint16_t>(hp.normal);
    buffer->write<uint16_t>(hp.armor);
    buffer->write<uint16_t>(hp.shields);

    pressed_keys.serialize(buffer);
    held_keys.serialize(buffer);
    buffer->write<int16_t>(mouse_x);
    buffer->write<int16_t>(mouse_y);

    getweapon(state)->serialize(state, buffer, fullupdate);
}

void Character::deserialize(Gamestate *state, ReadBuffer *buffer, bool fullupdate)
{
    MovingEntity::deserialize(state, buffer, fullupdate);

    hp.normal = buffer->read<uint16_t>();
    hp.armor = buffer->read<uint16_t>();
    hp.shields = buffer->read<uint16_t>();

    pressed_keys.deserialize(buffer);
    held_keys.deserialize(buffer);
    mouse_x = buffer->read<int16_t>();
    mouse_y = buffer->read<int16_t>();

    getweapon(state)->setaim(mouse_x, mouse_y);

    getweapon(state)->deserialize(state, buffer, fullupdate);
}

void Character::damage(Gamestate *state, double amount)
{
    if (hp.shields < amount)
    {
        amount -= hp.shields;
        hp.shields = 0;
        if (hp.armor > 0)
        {
            amount -= std::min(amount/2.0, 5.0);
        }
        if (hp.armor < amount)
        {
            amount -= hp.armor;
            hp.armor = 0;
            hp.normal -= amount;
            if (hp.normal <= 0)
            {
                // RIP
                if (state->engine->isserver)
                {
                    destroy(state);

                    state->sendbuffer->write<uint8_t>(PLAYER_DIED);
                    state->sendbuffer->write<uint8_t>(state->findplayerid(owner));

                    state->get<Player>(owner)->spawntimer.reset();
                }
            }
        }
        else
        {
            hp.armor -= amount;
        }
    }
    else
    {
        hp.shields -= amount;
    }
}

void Character::destroy(Gamestate *state)
{
    state->get<Player>(owner)->character = 0;
    getweapon(state)->destroy(state);
    Corpse *c = state->get<Corpse>(state->make_entity<Corpse>(state, getcharacterfolder(), isflipped));
    c->x = x;
    c->y = y;

    MovingEntity::destroy(state);
}

Weapon* Character::getweapon(Gamestate *state)
{
    return state->get<Weapon>(weapon);
}
