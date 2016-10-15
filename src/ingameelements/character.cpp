#include <vector>
#include <cmath>

#include "ingameelements/character.h"
#include "gamestate.h"
#include "ingameelements/movingentity.h"
#include "player.h"
#include "global_constants.h"
#include "engine.h"
#include "ingameelements/weapon.h"

Character::Character(uint64_t id_, Gamestate *state, EntityPtr owner_, CharacterChildParameters parameters) : MovingEntity(id_, state),
            owner(owner_), weapon(parameters.weapon), hp(parameters.maxhp), maxhp(parameters.maxhp), hpdir(-1), pressed_keys(), held_keys(), lastdirectionpressed(0)
{
    acceleration = 300;
    runpower = parameters.runpower;
    // friction factor per second of null movement; calculated directly from Gang Garrison 2
    // from pygg2
    friction = 0.01510305449388463132584804061124;
}

Character::~Character()
{
    ;
}

void Character::setinput(Gamestate *state, INPUT_CONTAINER pressed_keys_, INPUT_CONTAINER held_keys_, double mouse_x_, double mouse_y_)
{
    pressed_keys = pressed_keys_;
    held_keys = held_keys_;
    mouse_x = mouse_x_;
    mouse_y = mouse_y_;
    Weapon *w = state->get<Weapon>(weapon);
    w->setaim(mouse_x, mouse_y);
}

void Character::beginstep(Gamestate *state, double frametime)
{
    state->get<Weapon>(weapon)->beginstep(state, frametime);
}

void Character::midstep(Gamestate *state, double frametime)
{
    if (cangetinput(state))
    {
        double maxhspeed;
        if (animstate()->crouchanim.active())
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
            if (not animstate()->crouchanim.active())
            {
                animstate()->crouchanim.active(true);
                animstate()->crouchanim.reset();
            }
        }
        else if (animstate()->crouchanim.active())
        {
            // We're crouched and we'd like to uncrouch
            // Do so only if we have the room
            if (not state->currentmap->collides(getstandingcollisionrect(state)))
            {
                animstate()->crouchanim.active(false);
            }
        }

        if (held_keys.RELOAD)
        {
            Weapon *w = state->get<Weapon>(weapon);
            w->reload(state, frametime);
        }
        // Shooting
        if (held_keys.PRIMARY_FIRE and state->engine->isserver)
        {
            Weapon *w = state->get<Weapon>(weapon);
            w->fireprimary(state, frametime);
        }
        if (held_keys.SECONDARY_FIRE and state->engine->isserver)
        {
            x = mouse_x;
            y = mouse_y;
            Weapon *w = state->get<Weapon>(weapon);
            w->firesecondary(state, frametime);
        }

        if (animstate()->isflipped != (mouse_x < 0))
        {
            // Spinjumping (compensate for later gravity)
            vspeed -= 540.0*frametime*3.0/4.0;
            animstate()->isflipped = (mouse_x < 0);
        }
    }

    hp += 20*hpdir*frametime;
    if (hp <= 0)
    {
        hp = 0;
        hpdir = 1;
    }
    else if (hp >= maxhp)
    {
        hp = maxhp;
        hpdir = -1;
    }

    // Gravity
    vspeed += 540.0*frametime;

    // apply friction
    hspeed *= std::pow(friction, frametime);

    state->get<Weapon>(weapon)->midstep(state, frametime);
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
        if (animstate()->isflipped)
        {
            animstate()->runanim.update(state, -hspeed*frametime);
            animstate()->crouchanim.update(state, -hspeed*frametime);
        }
        else
        {
            animstate()->runanim.update(state, hspeed*frametime);
            animstate()->crouchanim.update(state, hspeed*frametime);
        }
    }
    if (hspeed == 0.0)
    {
        bool run=animstate()->runanim.active(), crouch=animstate()->crouchanim.active();
        animstate()->runanim.reset();
        animstate()->runanim.active(run);
        animstate()->crouchanim.reset();
        animstate()->crouchanim.active(crouch);
    }

    state->get<Weapon>(weapon)->endstep(state, frametime);
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
        animstate()->crouchanim.active(prev_e->animstate()->crouchanim.active());
    }
    else
    {
        held_keys = next_e->held_keys;
        pressed_keys = next_e->pressed_keys;
        animstate()->crouchanim.active(prev_e->animstate()->crouchanim.active());
    }
    mouse_x = prev_e->mouse_x + alpha*(next_e->mouse_x - prev_e->mouse_x);
    mouse_y = prev_e->mouse_y + alpha*(next_e->mouse_y - prev_e->mouse_y);
    animstate()->runanim.interpolate(&(prev_e->animstate()->runanim), &(next_e->animstate()->runanim), alpha);
    hp = prev_e->hp + alpha*(next_e->hp - prev_e->hp);
}
