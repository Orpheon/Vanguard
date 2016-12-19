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
        CharacterChildParameters constructparameters(uint64_t id_, Gamestate *state, EntityPtr owner_) override;
        std::string getcharacterfolder() {return "heroes/mccree/";}
        bool cangetinput(Gamestate *state) override {return not rollanim.active() and Character::cangetinput(state);}
        Health getmaxhp() override;
        void useability1(Gamestate *state) override;
        void useability2(Gamestate *state) override;
        void useultimate(Gamestate *state) override;
        void resetafterult(Gamestate *state);
        void drawhud(Renderer *renderer, Gamestate *state) override;
        double passiveultcharge() override {return 20;}//0.4166666666666667;}
        bool weaponvisible(Gamestate *state) override {return (not rollanim.active() and not stunanim.active());}
        Heroclass heroclass() override {return MCCREE;}

        Animation rollanim;
        Animation flashbanganim;
        Timer rollcooldown;
        Timer flashbangcooldown;
        LoopAnimation ultwalkanim;
        Timer ulting;
    protected:
    private:
};

#endif // MCCREE_H
