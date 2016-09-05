#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>

#include "movingentity.h"
#include "datastructures.h"
#include "player.h"
#include "entityptr.h"


class Character : public MovingEntity
{
    public:
        Character(Gamestate *state, int ownerid);
        virtual ~Character();
        virtual void setinput(INPUT_CONTAINER pressed_keys, INPUT_CONTAINER held_keys);
        virtual void beginstep(Gamestate *state, double frametime);
        virtual void midstep(Gamestate *state, double frametime);
        virtual void endstep(Gamestate *state, double frametime);
        virtual void clonedata(Character *c);

        PlayerPtr owner;

    protected:
        std::shared_ptr<ALLEGRO_BITMAP> sprite;
    private:
        enum INPUTBITS {LEFT, RIGHT, JUMP, CROUCH};
        unsigned int inputstate;
};

#endif // CHARACTER_H
