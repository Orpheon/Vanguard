#ifndef HAMMER_H
#define HAMMER_H

#include "ingameelements/weapon.h"

class Hammer : public Weapon
{
    public:
        Hammer(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~Hammer();

        void renderbehind(Renderer *renderer, Gamestate *state);
        void render(Renderer *renderer, Gamestate *state);
        std::string getsprite(Gamestate *state, bool mask) {return "";}
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Hammer(*this));}

        void fireprimary(Gamestate *state) override;
        void wantfireprimary(Gamestate *state) override;
        void firesecondary(Gamestate *state) override;
        void wantfiresecondary(Gamestate *state) override;
        void midstep(Gamestate *state, double frametime) override;
        void reload(Gamestate *state) override;

        int getclipsize() override {return 6;}
        double getattachpoint_x() override {return -3;}
        double getattachpoint_y() override {return 8;}

        WeaponChildParameters constructparameters(Gamestate *state);

    protected:
    private:
        double DAMAGE = 70;
};

#endif // HAMMER_H
