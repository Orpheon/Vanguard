#pragma once

#include "datastructures.h"
#include "gamestate.h"

class Projectile : public MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~Projectile() override = default;
        virtual bool isrootobject() override {return true;}
        virtual Rect getrect() = 0;
        virtual void oncollision(Gamestate *state, Character *c);
        virtual void oncollision(Gamestate *state);
        virtual double damage() = 0;
        virtual void midstep(Gamestate *state, double frametime) override;
//        virtual bool collides(Gamestate *state, EntityPtr otherentity, double angle);
        EntityPtr owner;
        Team team;
    protected:
    private:
};

