#ifndef CHARACTER_H
#define CHARACTER_H

#include <movingentity.h>
#include <global_constants.h>


class Character : public MovingEntity
{
    public:
        Character();
        virtual ~Character();
        void setinput(bool left, bool right, bool jump, bool crouch);

    protected:
    private:
        enum INPUTBITS {LEFT, RIGHT, JUMP, CROUCH};
        unsigned int inputstate;
};

#endif // CHARACTER_H
