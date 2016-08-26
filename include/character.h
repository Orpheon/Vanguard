#ifndef CHARACTER_H
#define CHARACTER_H

#include "movingentity.h"
#include "global_constants.h"


class Character : public MovingEntity
{
    public:
        Character(Gamestate *state);
        virtual ~Character();
        virtual void setinput(bool left, bool right, bool jump, bool crouch);
        virtual void beginstep(Gamestate *state, double frametime);
        virtual void midstep(Gamestate *state, double frametime);
        virtual void endstep(Gamestate *state, double frametime);
        virtual void clonedata(Character *c);

    protected:
    private:
        enum INPUTBITS {LEFT, RIGHT, JUMP, CROUCH};
        unsigned int inputstate;
};

#endif // CHARACTER_H
