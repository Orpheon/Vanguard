#ifndef CHARACTER_H
#define CHARACTER_H

#include <memory>
#include <string>

#include "ingameelements/movingentity.h"
#include "datastructures.h"
#include "ingameelements/player.h"
#include "animation.h"
#include "ingameelements/weapon.h"
#include "global_constants.h"

class Character : public MovingEntity
{
    public:
        Character(uint64_t id_, Gamestate *state, EntityPtr owner_, CharacterChildParameters parameters);
        virtual ~Character() = default;
        virtual void setinput(Gamestate *state, InputContainer held_keys_, double mouse_x_, double mouse_y_);
        virtual void beginstep(Gamestate *state, double frametime);
        virtual void midstep(Gamestate *state, double frametime);
        virtual void endstep(Gamestate *state, double frametime);
        virtual void render(Renderer *renderer, Gamestate *state);
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
        virtual bool cangetinput(Gamestate *state) {return not stunanim.active();}
        virtual void damage(Gamestate *state, double amount);
        Weapon *getweapon(Gamestate *state);
        virtual void useability1(Gamestate *state) = 0;
        virtual void useability2(Gamestate *state) = 0;
        virtual void drawhud(Renderer *renderer, Gamestate *state);
        virtual double passiveultcharge() = 0;
        virtual double hudheight() {return 7.0/8.0;}
        virtual bool weaponvisible(Gamestate *state) {return true;}
        virtual Heroclass heroclass() = 0;

        EntityPtr owner;
        EntityPtr weapon;

        double friction;
        double acceleration;
        double runpower;

        Health hp;
        virtual Health getmaxhp() = 0;

        Team team;

        bool isflipped;
        LoopAnimation runanim;
        LoopAnimation crouchanim;
        Animation stunanim;
        Timer ongroundsmooth;

    protected:
        InputContainer held_keys;
        double mouse_x;
        double mouse_y;
        const int LEFT = -1;
        const int RIGHT = 1;
};

#endif // CHARACTER_H
