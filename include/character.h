#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>
#include <string>

#include "movingentity.h"
#include "datastructures.h"
#include "player.h"
#include "animation.h"

class Character : public MovingEntity
{
    public:
        Character(Gamestate *state, PlayerPtr owner_, CharacterChildParameters arguments);
        virtual ~Character();
        virtual void setinput(INPUT_CONTAINER pressed_keys_, INPUT_CONTAINER held_keys_, double mouse_x_, double mouse_y_);
        virtual void beginstep(Gamestate *state, double frametime);
        virtual void midstep(Gamestate *state, double frametime);
        virtual void endstep(Gamestate *state, double frametime);

        virtual bool onground(Gamestate *state);
        virtual Rect getcollisionrect(Gamestate *state) = 0;
        virtual Rect getstandingcollisionrect(Gamestate *state) = 0;

        PlayerPtr owner;
        bool crouched;

    protected:
        INPUT_CONTAINER pressed_keys;
        INPUT_CONTAINER held_keys;
        double mouse_x;
        double mouse_y;
        LoopAnimation walkanim;

        virtual CharacterChildParameters constructCharacterChildParameters() = 0;
};

#endif // CHARACTER_H
