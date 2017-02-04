#ifndef CLIPWEAPON_H
#define CLIPWEAPON_H

#include "ingameelements/weapon.h"


class Clipweapon : public Weapon
{
    public:
        Clipweapon(uint64_t id_, Gamestate *state, EntityPtr owner_, WeaponChildParameters parameters);
        virtual ~Clipweapon();

        virtual void midstep(Gamestate *state, double frametime) override;
        virtual void interpolate(Entity *prev_entity, Entity *next_entity, double alpha) override;
        virtual void serialize(Gamestate *state, WriteBuffer *buffer, bool fullupdate) override;
        virtual void deserialize(Gamestate *state, ReadBuffer *buffer, bool fullupdate) override;
        bool hasclip() override {return true;}

        virtual void reload(Gamestate *state);
        virtual int getclipsize() = 0;
        virtual void restoreclip(Gamestate *state) {clip = getclipsize();}

        Animation reloadanim;
        int clip;

    protected:
    private:
};

#endif // CLIPWEAPON_H
