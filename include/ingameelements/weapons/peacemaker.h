#ifndef PEACEMAKER_H
#define PEACEMAKER_H

#include "ingameelements/weapon.h"
#include "ingameelements/clipweapon.h"
#include <unordered_map>

class Peacemaker : public Clipweapon
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
        void fireultimate(Gamestate *state);
        void midstep(Gamestate *state, double frametime) override;
        void reload(Gamestate *state) override;

        int getclipsize() override {return 6;}
        double getattachpoint_x() override {return -3;}
        double getattachpoint_y() override {return 8;}

        WeaponChildParameters constructparameters(Gamestate *state);
        Animation fthanim;
        bool isfthing;

        std::unordered_map<int, double> deadeyetargets;
        Animation deadeyeanim;
        bool isfiringult;
    protected:
    private:
        double MAX_DAMAGE = 70;
        double MAX_FTH_DAMAGE = 45;
        double FALLOFF_BEGIN = 30*10;
        double FALLOFF_END = 30*20;
};

#endif // PEACEMAKER_H
