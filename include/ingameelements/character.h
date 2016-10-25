#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>
#include <string>

#include "ingameelements/movingentity.h"
#include "datastructures.h"
#include "ingameelements/player.h"
#include "animation.h"
#include "ingameelements/weapon.h"

class Character : public MovingEntity
{
    public:
        Character(uint64_t id_, Gamestate *state, EntityPtr owner_, CharacterChildParameters parameters);
        virtual ~Character() = default;
        virtual void setinput(Gamestate *state, INPUT_CONTAINER pressed_keys_, INPUT_CONTAINER held_keys_, double mouse_x_, double mouse_y_);
        virtual void beginstep(Gamestate *state, double frametime);
        virtual void midstep(Gamestate *state, double frametime);
        virtual void endstep(Gamestate *state, double frametime);
        bool isrootobject() {return false;}
        virtual void interpolate(Entity *prev_entity, Entity *next_entity, double alpha);
        virtual void serialize(Gamestate *state, WriteBuffer *buffer, bool fullupdate);
        virtual void deserialize(Gamestate *state, ReadBuffer *buffer, bool fullupdate);
        virtual void destroy(Gamestate *state);

        virtual bool onground(Gamestate *state);
        virtual Rect getcollisionrect(Gamestate *state) = 0;
        virtual Rect getstandingcollisionrect(Gamestate *state) = 0;
        virtual CharacterChildParameters constructparameters(uint64_t id_, Gamestate *state, EntityPtr owner_) = 0;
        virtual std::string getcharacterfolder() = 0;
        virtual bool cangetinput(Gamestate *state) {return true;}
        virtual void damage(double amount);
        Weapon *getweapon(Gamestate *state);
        virtual double getweaponpos_x() = 0;
        virtual double getweaponpos_y() = 0;
        virtual void useability1(Gamestate *state) = 0;
        virtual void useability2(Gamestate *state) = 0;

        EntityPtr owner;
        EntityPtr weapon;

        double friction;
        double acceleration;
        double runpower;

        Health hp;
        virtual Health getmaxhp() = 0;
        int hpdir = -1;

        bool isflipped;
        LoopAnimation runanim;
        LoopAnimation crouchanim;

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
