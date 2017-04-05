#pragma once

#include "ingameelements/weapon.h"

class Hammer : public Weapon
{
    public:
        constexpr static double DAMAGE = 70;
        constexpr static double MAX_BARRIER_HEALTH = 2000;
        constexpr static double BARRIER_RECHARGE = 195;

        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
        virtual ~Hammer() override = default;

        void renderbehind(Renderer &renderer, Gamestate &state);
        void render(Renderer &renderer, Gamestate &state) override;
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Hammer(*this));}

        void fireprimary(Gamestate &state) override;
        void wantfireprimary(Gamestate &state) override;
        void firesecondary(Gamestate &state) override;
        void wantfiresecondary(Gamestate &state) override;
        void beginstep(Gamestate &state, double frametime) override;
        void midstep(Gamestate &state, double frametime) override;

        double getattachpoint_x() override {return 0;}
        double getattachpoint_y() override {return 8;}
        std::string herofolder() {return "heroes/reinhardt/";}

        Timer barrier;
        Timer barrierrecharge;
        Timer barrierbreak;
        double barrierhealth = MAX_BARRIER_HEALTH;

    protected:
    private:
};

