#ifndef PEACEMAKER_H
#define PEACEMAKER_H

#include "weapon.h"

class Peacemaker : public Weapon
{
    public:
        Peacemaker(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~Peacemaker();

        void render(Renderer *renderer, Gamestate *state);
        std::string getsprite(Gamestate *state, bool mask) {return "";}
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Peacemaker(*this));}

        void fireprimary(Gamestate *state, double frametime) override;
        void firesecondary(Gamestate *state, double frametime) override {}

        int getclipsize() {return 6;}
        WeaponChildParameters constructparameters(Gamestate *state);
    protected:
    private:
        double xoffset = 0;
        double yoffset = 0;
        double bulletspeed = 500.0;
};

#endif // PEACEMAKER_H
