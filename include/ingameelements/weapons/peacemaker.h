#ifndef PEACEMAKER_H
#define PEACEMAKER_H

#include "ingameelements/weapon.h"

class Peacemaker : public Weapon
{
    public:
        Peacemaker(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~Peacemaker();

        void render(Renderer *renderer, Gamestate *state);
        std::string getsprite(Gamestate *state, bool mask) {return "";}
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Peacemaker(*this));}

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
        Animation fthanim;
        bool isfthing;
    protected:
    private:
        double bulletspeed = 1000.0;
};

#endif // PEACEMAKER_H
