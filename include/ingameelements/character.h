#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>
#include <string>

#include "ingameelements/movingentity.h"
#include "datastructures.h"
#include "player.h"
#include "animation.h"

class Character : public MovingEntity
{
    public:
        Character(uint64_t id_, Gamestate *state, EntityPtr owner_, CharacterChildParameters parameters);
        virtual ~Character();
        virtual void setinput(Gamestate *state, INPUT_CONTAINER pressed_keys_, INPUT_CONTAINER held_keys_, double mouse_x_, double mouse_y_);
        virtual void beginstep(Gamestate *state, double frametime);
        virtual void midstep(Gamestate *state, double frametime);
        virtual void endstep(Gamestate *state, double frametime);
        bool isrootobject() {return false;}
        virtual void interpolate(Entity *prev_entity, Entity *next_entity, double alpha);

        virtual bool onground(Gamestate *state);
        virtual Rect getcollisionrect(Gamestate *state) = 0;
        virtual Rect getstandingcollisionrect(Gamestate *state) = 0;
        virtual CharacterChildParameters constructparameters(uint64_t id_, Gamestate *state) = 0;
        virtual bool cangetinput(Gamestate *state) {return true;}
        virtual double gethppercent() {return hp/maxhp;}

        EntityPtr owner;
        EntityPtr weapon;

        double friction;
        double acceleration;
        double runpower;

        double hp;
        int maxhp;
        int hpdir; // DEBUGTOOL

        struct AnimationState
        {
            bool isflipped;
            LoopAnimation runanim;
            LoopAnimation crouchanim;
            AnimationState(std::string characterfolder) : isflipped(false), runanim(characterfolder+"run/"), crouchanim(characterfolder+"crouchwalk/") {}
        };
        virtual AnimationState* animstate() = 0;

    protected:
        INPUT_CONTAINER pressed_keys;
        INPUT_CONTAINER held_keys;
        int lastdirectionpressed;
        double mouse_x;
        double mouse_y;
        const int LEFT = -1;
        const int RIGHT = 1;
};

#endif // CHARACTER_H
