#pragma once

#include "ingameelements/movingentity.h"

class Shield : MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate *state, std::string herofolder, bool isflipped_);
        virtual ~Shield() override = default;
        void beginstep(Gamestate *state, double frametime) override;
        void midstep(Gamestate *state, double frametime) override;
        void endstep(Gamestate *state, double frametime) override;
        void render(Renderer *renderer, Gamestate *state) override;
        bool isrootobject() override {return false;};
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Shield(*this));}
        void interpolate(Entity *prev_entity, Entity *next_entity, double alpha) override;
};