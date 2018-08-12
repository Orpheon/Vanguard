#pragma once

#include "ingameelements/character.h"
#include "ingameelements/weapons/pulserifle.h"
#include "renderer.h"
#include "datastructures.h"

class Soldier76 : public Character
{
public:
    virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_) override;
    virtual ~Soldier76() override = default;
    void beginstep(Gamestate &state, double frametime) override;
    void midstep(Gamestate &state, double frametime) override;
    void render(Renderer &renderer, Gamestate &state) override;
    void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
    Rect getcollisionrect(Gamestate &state) override;
    Rect getstandingcollisionrect(Gamestate &state) override;
    std::string currentsprite(Gamestate &state, bool mask) override;
    std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Soldier76(*this));}
    void useability1(Gamestate &state) override;
    void useability2(Gamestate &state) override;
    void useultimate(Gamestate &state) override;
    void interrupt(Gamestate &state) override;
    bool weaponvisible(Gamestate &state) override;

    Health initializehealth() override {return Health(200, 0, 0);}
    Heroclass heroclass() override {return SOLDIER76;}
    std::string herofolder() override {return "heroes/soldier76/";}
    double runpower() override {return 1.8;}
    EntityPtr constructweapon(Gamestate &state) {return state.make_entity<Pulserifle>(state, owner);}
protected:
private:
};