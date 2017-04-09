#pragma once

#include "ingameelements/shield.h"

class ReinhardtShield : public Shield
{
    public:
        void init(uint64_t id_, Gamestate &state, double maxhp, Team team_, EntityPtr owner_);
        virtual ~ReinhardtShield() override = default;
        virtual void beginstep(Gamestate &state, double frametime) override;
        virtual void midstep(Gamestate &state, double frametime) override;
        virtual void endstep(Gamestate &state, double frametime) override;
        virtual void render(Renderer &renderer, Gamestate &state) override;
        virtual std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new ReinhardtShield(*this));}
        virtual void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        virtual bool collides(Gamestate &state, double testx, double testy) override;
        virtual double maxdamageabledist(Gamestate &state, double *centerx, double *centery) override;

        EntityPtr owner;
};