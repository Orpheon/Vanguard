#pragma once

#include "ingameelements/projectile.h"

#include <deque>

class Firestrike : public Projectile
{
    public:
        void init(uint64_t id_, Gamestate &state, EntityPtr owner_);
        double dealdamage(Gamestate &state, Entity &target) override;
        PenetrationLevel penetration() override {return static_cast<PenetrationLevel>(PENETRATE_CHARACTER | PENETRATE_SHIELD);}
        std::string spriteid() override {return "heroes/reinhardt/firestrikeprojectile/1";}
        void endstep(Gamestate &state, double frametime) override {}
        void render(Renderer &renderer, Gamestate &state) override;
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Firestrike(*this));}

        double SPEED = 300;
    private:
        std::deque<EntityPtr> already_damaged_entities;
        double DAMAGE = 100;
};