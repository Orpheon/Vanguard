#pragma once

#include "ingameelements/projectile.h"

class SonicProj : public Projectile
{
    public:
        void init(uint64_t id_, Gamestate &state, EntityPtr owner_);
        double dealdamage(Gamestate &state, Entity &target) override;
        PenetrationLevel penetration() override {return static_cast<PenetrationLevel>(PENETRATE_NOTHING);}
        std::string spriteid() override {return "heroes/lucio/projectile/1";}
        void endstep(Gamestate &state, double frametime) override {}
        void render(Renderer &renderer, Gamestate &state) override;
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new SonicProj(*this));}

        double SPEED = 600;
    private:
        double DAMAGE = 20;
};