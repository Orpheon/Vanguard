#pragma once

#include "ingameelements/clipweapon.h"

class Sonicamp : public Clipweapon
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
        virtual ~Sonicamp() override = default;

        void renderbehind(Renderer &renderer, Gamestate &state);
        void render(Renderer &renderer, Gamestate &state) override;
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Sonicamp(*this));}
        void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        void midstep(Gamestate &state, double frametime) override;

        void fireprimary(Gamestate &state) override;
        void firesecondary(Gamestate &state) override;
        void wantfireprimary(Gamestate &state) override;
        void wantfiresecondary(Gamestate &state) override;
        bool canreload(Gamestate &state) override {return Clipweapon::canreload(state) and not refiredelay.active
                                                          and not postsoundwavedelay.active;}
        void createprojectile(Gamestate &state);

        std::string herofolder() override {return "heroes/lucio/";}
        int getclipsize() override {return 20;}
        std::function<void(Gamestate &state)> getreloadfunction(Gamestate &state) override {return std::bind(&Sonicamp::restoreclip, this, std::placeholders::_1);}
        double getattachpoint_x(Gamestate &state) override {return -5;}
        double getattachpoint_y(Gamestate &state) override {return 4;}
        double getbackattachpoint_x(Gamestate &state);
        double getbackattachpoint_y(Gamestate &state);

        Timer soundwavecooldown;
        Animation soundwave;
        Timer postsoundwavedelay;
        Timer refireloop;
        Timer refiredelay;
        int refirecounter;

        constexpr static int SOUNDWAVE_RANGE = 30 * 8;
        constexpr static int SOUNDWAVE_FORCE = 500;
    protected:
    private:
};