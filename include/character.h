#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>

#include "movingentity.h"
#include "datastructures.h"
#include "player.h"

class Character : public MovingEntity
{
    public:
        Character(Gamestate *state, PlayerPtr owner_);
        virtual ~Character();
        virtual void setinput(INPUT_CONTAINER pressed_keys_, INPUT_CONTAINER held_keys_, double mouse_x_, double mouse_y_);
        virtual void beginstep(Gamestate *state, double frametime);
        virtual void midstep(Gamestate *state, double frametime);
        virtual void endstep(Gamestate *state, double frametime);

        virtual bool onground(Gamestate *state);

        PlayerPtr owner;

    protected:
        INPUT_CONTAINER pressed_keys;
        INPUT_CONTAINER held_keys;
        double mouse_x;
        double mouse_y;
};

#endif // CHARACTER_H
