#include "character.h"
#include "gamestate.h"
#include "movingentity.h"


Character::Character(Gamestate *state) : MovingEntity(state)
{
    inputstate = 0;
}

Character::~Character()
{
    //dtor
}

void Character::setinput(bool left, bool right, bool jump, bool crouch)
{
    inputstate = (left<<INPUTBITS::LEFT) + (right<<INPUTBITS::RIGHT) + (jump<<INPUTBITS::JUMP) + (crouch<<INPUTBITS::CROUCH);
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
