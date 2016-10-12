#ifndef MCCREE_H
#define MCCREE_H

#include "character.h"
#include "renderer.h"
#include "datastructures.h"
#include "peacemaker.h"

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
        bool cangetinput(Gamestate *state) override {return not animstate()->rolling.active();}

        struct MccreeAnimationState : public AnimationState
        {
            Animation rolling;
            Animation flashbang;
            MccreeAnimationState() : AnimationState("heroes/mccree/"), rolling("heroes/mccree/roll/"), flashbang("heroes/mccree/flashbang/")
            {
                rolling.active(false);
                flashbang.active(false);
            }
        };
        MccreeAnimationState* animstate() override {return &animstate_;}
        Timer rollcooldown;
        Timer flashbangcooldown;
    protected:
    private:
        MccreeAnimationState animstate_;
};

#endif // MCCREE_H
