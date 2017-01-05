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
        void interpolate(Entity *prev_entity, Entity *next_entity, double alpha) override;
        Rect getcollisionrect(Gamestate *state) override;
        Rect getstandingcollisionrect(Gamestate *state) override;
        std::string getsprite(Gamestate *state, bool mask) override;
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Mccree(*this));}
        CharacterChildParameters constructparameters(uint64_t id_, Gamestate *state, EntityPtr owner_) override;
        std::string getcharacterfolder() {return "mccree/";}
        bool cangetinput(Gamestate *state) override {return not rollanim.active() and not ultcooldown.active and Character::cangetinput(state);}
        bool canuseweapons(Gamestate *state) override {return not ulting.active and Character::canuseweapons(state);}
        bool canuseabilities(Gamestate *state) override {return not ulting.active and Character::canuseabilities(state);}
        Health getmaxhp() override;
        void useability1(Gamestate *state) override;
        void useability2(Gamestate *state) override;
        void useultimate(Gamestate *state) override;
        void resetafterult(Gamestate *state);
        void stun(Gamestate *state) override;
        void drawhud(Renderer *renderer, Gamestate *state) override;
        double passiveultcharge() override {return 20;}//0.4166666666666667;}
        bool weaponvisible(Gamestate *state) override;
        Heroclass heroclass() override {return MCCREE;}
        double getmaxhspeed() override {return ulting.active ? 20 : Character::getmaxhspeed();}

        Animation rollanim;
        Animation flashbanganim;
        Timer rollcooldown;
        Timer flashbangcooldown;
        LoopAnimation ultwalkanim;
        Timer ulting;
        Timer ultcooldown;
    protected:
    private:
};

#endif // MCCREE_H
