#pragma once

#include "ingameelements/movingentity.h"

class Shield : MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate *state, std::string herofolder, bool isflipped_);
        virtual ~Shield() override = default;
        virtual void beginstep(Gamestate *state, double frametime) override;
        virtual void midstep(Gamestate *state, double frametime) override;
        virtual void endstep(Gamestate *state, double frametime) override;
        virtual void render(Renderer *renderer, Gamestate *state) override;
        virtual bool isrootobject() override {return false;};
        virtual void interpolate(Entity *prev_entity, Entity *next_entity, double alpha) override;
        virtual bool collides(Gamestate *state, double x_, double y_) override;
};