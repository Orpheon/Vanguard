#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "datastructures.h"
#include "gamestate.h"

class Projectile : public MovingEntity
{
    public:
        Projectile(double id_, Gamestate *state, EntityPtr owner_);
        virtual ~Projectile();
        virtual bool isrootobject() {return true;}
        virtual bool isrectangular() = 0;
        virtual Rect getrect() = 0;
        virtual double getradius() = 0;
        virtual void oncollision(Gamestate *state, Character *c);
        virtual void oncollision(Gamestate *state);
        virtual double damage() = 0;
        virtual void midstep(Gamestate *state, double frametime);
        virtual bool rectcollides(Gamestate *state, EntityPtr otherentity, double angle);
        virtual bool circlecollides(Gamestate *state, EntityPtr otherentity, double radius);
        EntityPtr owner;
    protected:
    private:
};

#endif // PROJECTILE_H
