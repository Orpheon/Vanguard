#include "character.h"

Character::Character()
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
