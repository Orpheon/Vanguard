#pragma once

#include "ingameelements/weapon.h"
#include "ingameelements/clipweapon.h"

class Pulserifle : public Clipweapon
{
public:
    virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
    virtual ~Pulserifle() override = default;

    void render(Renderer &renderer, Gamestate &state) override;
    std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Pulserifle(*this));}

    void fireprimary(Gamestate &state) override;
    void wantfireprimary(Gamestate &state) override;
    void firesecondary(Gamestate &state) override;
    void wantfiresecondary(Gamestate &state) override;
    void fireultimate(Gamestate &state);
    void beginstep(Gamestate &state, double frametime) override;
    void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
    void reload(Gamestate &state) override;

    std::string herofolder() override {return "heroes/soldier76/";}
    int getclipsize() override {return 25;}
    std::function<void(Gamestate &state)> getreloadfunction(Gamestate &state) override {return std::bind(&Pulserifle::restoreclip, this, std::placeholders::_1);}
    double getattachpoint_x(Gamestate &state) override {return 0;}
    double getattachpoint_y(Gamestate &state) override {return 0;}

    Timer shotspread;

protected:
private:
    double MAX_DAMAGE = 19;
    // Max spread in degrees
    double MAX_SPREAD = 10;
    double FALLOFF_BEGIN = 30*10;
    double FALLOFF_END = 50*20;
};

