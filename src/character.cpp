#include <vector>
#include <cmath>

#include "character.h"
#include "gamestate.h"
#include "movingentity.h"
#include "player.h"
#include "global_constants.h"
#include "engine.h"


Character::Character(Gamestate *state, PlayerPtr owner_, CharacterChildParameters arguments) : MovingEntity(state),
            owner(owner_), pressed_keys(), held_keys(), walkanim(arguments.walkanimpath)
{
    isflipped = false;
    crouched = false;
}

Character::~Character()
{
    ;
}

void Character::setinput(INPUT_CONTAINER pressed_keys_, INPUT_CONTAINER held_keys_, double mouse_x_, double mouse_y_)
{
    pressed_keys = pressed_keys_;
    held_keys = held_keys_;
    mouse_x = mouse_x_;
    mouse_y = mouse_y_;
}

void Character::beginstep(Gamestate *state, double frametime)
{
    ;
}

void Character::midstep(Gamestate *state, double frametime)
{
    if (held_keys.LEFT)
    {
        hspeed = std::max(hspeed-300*frametime, -153.0);
    }
    if (held_keys.RIGHT)
    {
        hspeed = std::min(hspeed+300*frametime, 153.0);
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
        crouched = true;
    }
    else if (crouched)
    {
        // We're crouched and we'd like to uncrouch
        // Do so only if we have the room
        if (not state->currentmap->collides(state, getstandingcollisionrect(state)))
        {
            crouched = false;
        }
    }

    isflipped = (mouse_x < 0);
    vspeed += 540.0*frametime;
}

void Character::endstep(Gamestate *state, double frametime)
{
    MovingEntity::endstep(state, frametime);

    // Collision with wallmask
    if (state->currentmap->collides(state, getcollisionrect(state)))
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
            if (not state->currentmap->collides(state, getcollisionrect(state)))
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
                if (not state->currentmap->collides(state, getcollisionrect(state).offset(xstep, 0)))
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
                    if (not state->currentmap->collides(state, getcollisionrect(state).offset(xstep, STAIRCASE_STEPSIZE)))
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
                if (not state->currentmap->collides(state, getcollisionrect(state).offset(0, ystep)))
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
        if (state->currentmap->collides(state, getcollisionrect(state)))
        {
            printf("\nError: Still in wallmask after collision handling.");
        }
    } // end collision with wallmask


    // Walking animation
    if (isflipped)
    {
        walkanim.update(-hspeed*frametime);
    }
    else
    {
        walkanim.update(hspeed*frametime);
    }
    if (hspeed == 0.0)
    {
        walkanim.reset();
    }
}

bool Character::onground(Gamestate *state)
{
    Rect r = getcollisionrect(state);
    return state->currentmap->collides(state, Rect(r.x, r.y+r.h, r.w, 1));
}
