#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <ingameelements/movingentity.h>
#include "animation.h"


class Explosion : public MovingEntity
{
    public:
        Explosion(uint64_t id_, Gamestate *state, std::string animationpath);
        virtual ~Explosion();
        void beginstep(Gamestate *state, double frametime) {}
        void midstep(Gamestate *state, double frametime);
        void endstep(Gamestate *state, double frametime) {}
        std::string getsprite(Gamestate *state, bool mask) {return explosionanim.getframe();}
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Explosion(*this));}
        void render(Renderer *renderer, Gamestate *state);
        bool isrootobject() {return true;}

        Animation explosionanim;
    protected:
    private:
};

#endif // EXPLOSION_H
