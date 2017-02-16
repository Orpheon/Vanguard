#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <ingameelements/movingentity.h>
#include "animation.h"


class Explosion : public MovingEntity
{
    public:
        virtual void init(uint64_t id_, Gamestate *state, std::string animationpath, double direction_);
        virtual ~Explosion() override;
        void beginstep(Gamestate *state, double frametime) override {}
        void midstep(Gamestate *state, double frametime) override;
        void endstep(Gamestate *state, double frametime) override {}
        std::string getsprite(Gamestate *state, bool mask) override {return explosionanim.getframepath();}
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Explosion(*this));}
        void render(Renderer *renderer, Gamestate *state) override;
        bool isrootobject() override {return true;}

        Animation explosionanim;
        double direction;
    protected:
    private:
};

#endif // EXPLOSION_H
