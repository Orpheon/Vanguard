#ifndef WEAPON_H
#define WEAPON_H

#include "movingentity.h"
#include "datastructures.h"


class Weapon : public MovingEntity
{
    public:
        Weapon(Gamestate *state, EntityPtr owner_);
        virtual ~Weapon();
        void setaim(double x, double y);

        bool isrootobject() {return false;}

        EntityPtr owner;
        double aimdirection;

        void beginstep(Gamestate *state, double frametime);
        void midstep(Gamestate *state, double frametime);
        void endstep(Gamestate *state, double frametime);
        std::unique_ptr<Entity> clone();

        std::string idlesprite;
    protected:
    private:
};

#endif // WEAPON_H
