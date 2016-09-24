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

    isflipped = (mouse_x < 0);
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
                    // Check if we collided with a staircase
                    y += STAIRCASE_STEPSIZE;
                    if (state->currentmap->collides(state, this))
                    {
                        // Still doesn't work, revert step testing and mark coordinate as blocked
                        x -= xstep;
                        y -= STAIRCASE_STEPSIZE;
                        xblocked = true;
                        xfinished = true;
                    }
                }
                if (not xblocked)
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
            // Do the same vertically, but without stair code
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
        if (state->currentmap->collides(state, this))
        {
            // We're still stuck
            // We got placed into the wallmask without moving into it
            // This is only possible through sprite/mask changes while player is immobile
            // Since the mask changes are never so drastic as to place you completely somewhere different, we're probably very close to freedom
            // Therefore, it's enough to check around us for the closest airspace, and move there

            // Check horizontally first
            int left_offset = state->engine->maskloader.get_spriteoffset_x(getmask());
            int right_offset = al_get_bitmap_width(state->engine->maskloader.request_sprite(getmask())) - left_offset;
            int top_offset = state->engine->maskloader.get_spriteoffset_y(getmask());
            int bot_offset = al_get_bitmap_height(state->engine->maskloader.request_sprite(getmask())) - top_offset;
            int dir_x = 0;
            bool finished = false;

            // Go through the entire two vertical sides and check which side is more free
            for (int i=-top_offset; i<=bot_offset; ++i)
            {
                if (state->currentmap->place_free(x-left_offset-1, y+i))
                {
                    dir_x -= 1;
                }
                if (state->currentmap->place_free(x+right_offset+1, y+i))
                {
                    dir_x += 1;
                }
            }
            // Normalize to -1, 0 or +1
            dir_x = (dir_x > 0) - (dir_x < 0);
            if (dir_x != 0)
            {
                // We can try to escape by moving horizontally the width of our current mask sprite
                // This should be enough for regular turning around
                for (int i=0; i<left_offset+right_offset; ++i)
                {
                    x += dir_x;
                    if (not state->currentmap->collides(state, this))
                    {
                        // We're done
                        finished = true;
                        break;
                    }
                }
                if (not finished)
                {
                    // Reverse what we just tried, to try two dimensions later
                    x -= left_offset+right_offset;
                }
            }
            if (not finished)
            {
                // Checking just horizontally wasn't enough
                // Now we need to check vertically as well
                int dir_y = 0;
                for (int i=-left_offset; i<=right_offset; ++i)
                {
                    if (state->currentmap->place_free(x+i, y-top_offset-1))
                    {
                        dir_y -= 1;
                    }
                    if (state->currentmap->place_free(x+i, y+bot_offset+1))
                    {
                        dir_y += 1;
                    }
                }
                // Normalize to -1, 0 or +1
                dir_y = (dir_y > 0) - (dir_y < 0);
                if (dir_y == 0)
                {
                    printf("\nERROR: Collision could not be resolved!");
                    printf("\nNone of the 4 directions was detected as free to move out to!");
                    printf("\n");
                }
                // Move out. If one of the horizontal paths was open but not sufficient to get us out, we are in a corner
                // Otherwise, if just stuck in ceiling/floor, dir_x should be 0 and this compensated anyway
                for (int i=0; i<std::fmax(left_offset+right_offset, top_offset+bot_offset); ++i)
                {
                    x += dir_x; y += dir_y;
                    if (not state->currentmap->collides(state, this))
                    {
                        // We're done
                        finished = true;
                        break;
                    }
                }
                if (not finished)
                {
                    printf("\nERROR: Collision could not be resolved!");
                    printf("\nExtraction was attempted but failed!");
                    printf("\ndir_x: %i\tdir_y: %i", dir_x, dir_y);
                    printf("\n");
                }
            }
        }
        else // if we got out of collision through movement correction
        {
            // Set hspeed and vspeed to what they should be
            if (xblocked)
            {
                hspeed = 0;
            }
            if (yblocked)
            {
                vspeed = 0;
            }
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
    y += 1;
    bool result = state->currentmap->collides(state, this);
    y -= 1;
    return result;
}

std::string Character::getmask()
{
    return walkanim.get_frame();
}
