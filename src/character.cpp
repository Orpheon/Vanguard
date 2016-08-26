#include "character.h"
#include "gamestate.h"
#include "movingentity.h"
#include "player.h"

Character::Character(Gamestate *state, Player *owner) : MovingEntity(state)
{
    inputstate = 0;
    this->owner = owner;
}

Character::~Character()
{
    owner->character = 0;
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
    ;
}

void Character::endstep(Gamestate *state, double frametime)
{
    MovingEntity::endstep(state, frametime);
}

void Character::clonedata(Character *c)
{
    MovingEntity::clonedata(c);
    c->inputstate = inputstate;
}
