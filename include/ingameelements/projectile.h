#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "datastructures.h"
#include "gamestate.h"

class Projectile : public MovingEntity
{
    public:
        Projectile(double id_, Gamestate *state, EntityPtr owner_);
        virtual ~Projectile();

        virtual void beginstep(Gamestate *state, double frametime);
        virtual void midstep(Gamestate *state, double frametime);
        virtual void endstep(Gamestate *state, double frametime);

        virtual bool isrootobject() {return true;}

        EntityPtr owner;
    protected:
    private:
};

#endif // PROJECTILE_H
