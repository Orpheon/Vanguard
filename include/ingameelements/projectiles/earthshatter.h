#pragma once

#include "gamestate.h"

class Earthshatter : public MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate &state, EntityPtr owner_);
        virtual void beginstep(Gamestate &state, double frametime) override;
        virtual void midstep(Gamestate &state, double frametime) override {};
        virtual void render(Renderer &renderer, Gamestate &state) override {};
        virtual bool isrootobject() override {return true;}
        virtual std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Earthshatter(*this));}
        virtual void interpolate(Entity &prev_entity, Entity &next_entity, double alpha) override;
        virtual bool isowner(EntityPtr potential_owner) override {return potential_owner == owner;}
        virtual void spawnexplosion(Gamestate &state);

        EntityPtr owner;
        Timer distance;
        int olddistance = 0;
        Timer explosionspawner;
        int direction;

        double MAX_RANGE = 7 * 30;
        double SPEED = MAX_RANGE / 0.5;
        double EXPLOSION_STEPSIZE = 20;
};