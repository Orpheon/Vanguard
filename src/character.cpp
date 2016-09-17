#include <vector>

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
        hspeed -= 140*frametime;
    }
    if (held_keys.RIGHT)
    {
        hspeed += 140*frametime;
    }

    if (held_keys.JUMP)
    {
        vspeed -= 140*frametime;
    }
    if (held_keys.CROUCH)
    {
        vspeed += 140*frametime;
    }
}

void Character::endstep(Gamestate *state, double frametime)
{
    MovingEntity::endstep(state, frametime);

    // Collision with wallmask
    printf("\n%s", state->currentmap->collides(state, this) ? "true" : "false");
}
