#include <vector>
#include <cmath>

#include "character.h"
#include "gamestate.h"
#include "movingentity.h"
#include "player.h"



Character::Character(Gamestate *state, PlayerPtr owner_) : MovingEntity(state), owner(owner_), pressed_keys(), held_keys()
{
    ;
}

Character::~Character()
{
    ;
}

void Character::setinput(INPUT_CONTAINER pressed_keys_, INPUT_CONTAINER held_keys_)
{
    pressed_keys = pressed_keys_;
    held_keys = held_keys_;
}

void Character::beginstep(Gamestate *state, double frametime)
{
    ;
}

void Character::midstep(Gamestate *state, double frametime)
{
    if (held_keys.LEFT)
    {
        hspeed = std::max(hspeed-300*frametime, -450.0);
    }
    if (held_keys.RIGHT)
    {
        hspeed = std::min(hspeed+300*frametime, 450.0);
    }
    if (pressed_keys.JUMP)
    {
        if (onground(state))
        {
            vspeed = -240.0;
        }
    }

    vspeed += 540.0*frametime;
}

void Character::endstep(Gamestate *state, double frametime)
{
    MovingEntity::endstep(state, frametime);

    // Collision with wallmask
    if (state->currentmap->collides(state, this))
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
            if (not state->currentmap->collides(state, this))
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
                x += xstep;
                if (state->currentmap->collides(state, this))
                {
                    // Doesn't work
                    x -= xstep;
                    xblocked = true;
                    xfinished = true;
                }
                else
                {
                    // It did work, deduct this from distance still to travel
                    xbuffer -= xstep;
                    if (xbuffer == 0.0 or std::signbit(xbuffer) != std::signbit(oldxbuffer))
                    {
                        // We're done for this coordinate
                        xfinished = true;
                    }
                }
            }
            // Do the same vertically
            if (not yfinished)
            {
                y += ystep;
                if (state->currentmap->collides(state, this))
                {
                    // Doesn't work
                    y -= ystep;
                    yblocked = true;
                    yfinished = true;
                }
                else
                {
                    // It did work, deduct this from distance still to travel
                    ybuffer -= ystep;
                    if (ybuffer == 0.0 or std::signbit(ybuffer) != std::signbit(oldybuffer))
                    {
                        // We're done for this coordinate
                        yfinished = true;
                    }
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
        if (state->currentmap->collides(state, this))
        {
            printf("\nERROR: Character still stuck after collision reaction.");
        }
    }
}

bool Character::onground(Gamestate *state)
{
    y += 1;
    bool result = state->currentmap->collides(state, this);
    y -= 1;
    return result;
}
