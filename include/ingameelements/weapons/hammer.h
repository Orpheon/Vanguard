#pragma once

#include "ingameelements/weapon.h"
#include "ingameelements/weapons/reinhardtshield.h"

class Hammer : public Weapon
{
    public:
        constexpr static double DAMAGE = 70;

        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
        virtual ~Hammer() override = default;

        void renderbehind(Renderer &renderer, Gamestate &state);
        void render(Renderer &renderer, Gamestate &state) override;
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Hammer(*this));}

        void fireprimary(Gamestate &state) override;
        void wantfireprimary(Gamestate &state) override;
        void firesecondary(Gamestate &state) override;
        void wantfiresecondary(Gamestate &state) override;
        void createfirestrike(Gamestate &state);
        void hitarea(Gamestate &state);
        void beginstep(Gamestate &state, double frametime) override;
        void midstep(Gamestate &state, double frametime) override;
        void endstep(Gamestate &state, double frametime) override;
        void destroy(Gamestate &state) override;

        double getattachpoint_x(Gamestate &state) override;
        double getattachpoint_y(Gamestate &state) override;
        double getbackattachpoint_x(Gamestate &state);
        double getbackattachpoint_y(Gamestate &state);
        std::string herofolder() {return "heroes/reinhardt/";}

        ReinhardtShield& barrier(Gamestate &state);

        EntityPtr barrierptr;
        Animation firestrikeanim;
        Timer firestrikedelay;
        Timer firingdelay1;
        Timer firingdelay2;

    protected:
    private:
};

