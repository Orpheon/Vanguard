#pragma once

#include <ingameelements/projectile.h>


class Flashbang : public Projectile
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
        virtual ~Flashbang() override = default;
        void beginstep(Gamestate &state, double frametime) override {}
        void midstep(Gamestate &state, double frametime) override;
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Flashbang(*this));}
        void render(Renderer &renderer, Gamestate &state) override;
        double explode(Gamestate &state);
        void hitwall(Gamestate &state);
        double dealdamage(Gamestate &state, Entity &target) override {return explode(state);}
        PenetrationLevel penetration() override {return PENETRATE_NOTHING;}
        std::string spriteid() override {return "heroes/mccree/projectiles/flashbang";}

        Timer countdown;
    protected:
    private:
        double EXPLOSION_RADIUS = 40;
};

