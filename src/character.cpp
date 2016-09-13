#include <vector>

#include "character.h"
#include "gamestate.h"
#include "movingentity.h"
#include "player.h"

Character::Character(Gamestate *state, PlayerPtr owner_) : MovingEntity(state), owner(owner_)
{
    inputstate = 0;
}

Character::~Character()
{
    ;
}

void Character::setinput(INPUT_CONTAINER pressed_keys, INPUT_CONTAINER held_keys)
{
    // FIXME: Should JUMP really be in here?
    inputstate = (held_keys.LEFT << INPUTBITS::LEFT)
               + (held_keys.RIGHT << INPUTBITS::RIGHT)
               + (held_keys.JUMP << INPUTBITS::JUMP)
               + (held_keys.CROUCH << INPUTBITS::CROUCH);
}

void Character::beginstep(Gamestate *state, double frametime)
{
    ;
}

void Character::midstep(Gamestate *state, double frametime)
{
    // Gravity
    vspeed += 140*frametime;
}

void Character::endstep(Gamestate *state, double frametime)
{
    MovingEntity::endstep(state, frametime);
}
