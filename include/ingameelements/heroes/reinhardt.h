#ifndef REINHARDT_H
#define REINHARDT_H

#include "ingameelements/character.h"
#include "renderer.h"
#include "datastructures.h"
#include "ingameelements/weapons/hammer.h"

#include <string>

class Reinhardt : public Character
{
    public:
        Reinhardt(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~Reinhardt() override;
        void midstep(Gamestate *state, double frametime) override;
        void render(Renderer *renderer, Gamestate *state) override;
        void interpolate(Entity *prev_entity, Entity *next_entity, double alpha) override;
        Rect getcollisionrect(Gamestate *state) override;
        Rect getstandingcollisionrect(Gamestate *state) override;
        std::string getsprite(Gamestate *state, bool mask) override;
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Reinhardt(*this));}
        CharacterChildParameters constructparameters(uint64_t id_, Gamestate *state, EntityPtr owner_) override;
        std::string getcharacterfolder() {return "reinhardt/";}
        bool cangetinput(Gamestate *state) override;
        Health getmaxhp() override;
        void useability1(Gamestate *state) override;
        void useability2(Gamestate *state) override;
        void useultimate(Gamestate *state) override;
        void begincharge() {chargeanim.reset();}
        void endcharge() {chargeanim.active(false); endchargeanim.reset();}
        void interrupt(Gamestate *state) override;
        void drawhud(Renderer *renderer, Gamestate *state) override;
        double passiveultcharge() override {return 100*0.4166666666666667;}
        bool weaponvisible(Gamestate *state) override;
        Heroclass heroclass() override {return REINHARDT;}
        double getmaxhspeed(Gamestate *state) override {return state->get<Hammer>(weapon)->barrier.active ? 60.0 : Character::getmaxhspeed(state);}

        Animation chargeanim;
        Animation preparechargeanim;
        Animation endchargeanim;
        Animation earthshatteranim;
    protected:
    private:
};

#endif // REINHARDT_H
