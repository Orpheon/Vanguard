#pragma once

#include "entity.h"
#include "datastructures.h"
#include "timer.h"
#include "ingameelements/gamemodes/gamemodemanager.h"

class ControlPoint : public Entity
{
public:
    virtual void init(uint64_t id_, Rect area_, uint8_t cpIndex_);
    virtual ~ControlPoint() override = default;
    void beginstep(Gamestate &state, double frametime) override;
    void render(Renderer &renderer, Gamestate &state) override;

    bool isinside(double x, double y) { return (x > area.x and x < area.x + area.w and y > area.y and y < area.y + area.h); }
    std::unique_ptr<Entity> clone() override { return std::unique_ptr<Entity>(new ControlPoint(*this)); }
    bool isrootobject() override { return true; }
    void midstep(Gamestate &state, double frametime) override;
    void endstep(Gamestate &state, double frametime) override {}
    void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override {}

    void capture(Team cappedteam, std::shared_ptr<GameModeManager> gamemanager);
    void lock();
    void unlock();

    Rect area;
    uint8_t cpindex = 0;
    Team cappingteam = SPECTATOR;
    bool locked = true;
    const int maxcap = 100;
    double cpidletimer = 0;
    double cappedamount[2] = {0, 0};
    Timer timer;
private:
};
