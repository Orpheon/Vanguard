#ifndef CORPSE_H
#define CORPSE_H

#include <ingameelements/movingentity.h>
#include "timer.h"

class Corpse : public MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate *state, std::string herofolder, bool isflipped_);
        virtual ~Corpse() override;
        void beginstep(Gamestate *state, double frametime) override {}
        void midstep(Gamestate *state, double frametime) override;
        void endstep(Gamestate *state, double frametime) override {}
        void render(Renderer *renderer, Gamestate *state) override;
        bool isrootobject() override {return true;};
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Corpse(*this));}
        void interpolate(Entity *prev_entity, Entity *next_entity, double alpha) override;
        std::string getsprite(Gamestate *state, bool mask) override {return spritepath;}
    protected:
    private:
        std::string spritepath;
        Timer countdown;
        bool isflipped;
};

#endif // CORPSE_H
