#ifndef WEAPON_H
#define WEAPON_H

#include "movingentity.h"
#include "datastructures.h"
#include "animation.h"


class Weapon : public MovingEntity
{
    public:
        Weapon(uint64_t id_, Gamestate *state, EntityPtr owner_, WeaponChildParameters parameters);
        virtual ~Weapon();
        virtual void setaim(double x_, double y_);

        bool isrootobject() {return false;}

        EntityPtr owner;
        double aimdirection;

        virtual void beginstep(Gamestate *state, double frametime);
        virtual void midstep(Gamestate *state, double frametime);
        virtual void endstep(Gamestate *state, double frametime);
        virtual void interpolate(Entity *prev_entity, Entity *next_entity, double alpha);

        virtual void fireprimary(Gamestate *state, double frametime) = 0;
        virtual void firesecondary(Gamestate *state, double frametime) = 0;
        virtual int getclipsize() = 0;
        virtual void reload(Gamestate *state, double frametime);
        virtual WeaponChildParameters constructparameters(Gamestate *state) = 0;

        void restoreclip(Gamestate *state) {clip = getclipsize();}

        std::string idlesprite;
        Animation firinganim;
        Animation reloadanim;
        int clip;
    protected:
    private:
};

#endif // WEAPON_H
