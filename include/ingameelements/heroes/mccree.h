#ifndef MCCREE_H
#define MCCREE_H

#include "ingameelements/character.h"
#include "renderer.h"
#include "datastructures.h"
#include "ingameelements/weapons/peacemaker.h"

#include <string>

class Mccree : public Character
{
    public:
        Mccree(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~Mccree() override;
        void midstep(Gamestate *state, double frametime) override;
        void render(Renderer *renderer, Gamestate *state) override;
        Rect getcollisionrect(Gamestate *state) override;
        Rect getstandingcollisionrect(Gamestate *state) override;
        std::string getsprite(Gamestate *state, bool mask) override;
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Mccree(*this));}
        CharacterChildParameters constructparameters(uint64_t id_, Gamestate *state) override;
        std::string getcharacterfolder() {return "heroes/mccree/";}
        bool cangetinput(Gamestate *state) override {return not rollanim.active();}
        Health getmaxhp() override;
        double getweaponpos_x() override {return -2;}
        double getweaponpos_y() override {return 7;}

        Animation rollanim;
        Animation flashbanganim;
        Timer rollcooldown;
        Timer flashbangcooldown;
    protected:
    private:
};

#endif // MCCREE_H
