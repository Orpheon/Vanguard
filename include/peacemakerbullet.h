#ifndef PEACEMAKERBULLET_H
#define PEACEMAKERBULLET_H

#include "projectile.h"
#include <string>

class PeacemakerBullet : public Projectile
{
    public:
        PeacemakerBullet(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~PeacemakerBullet();
        void midstep(Gamestate *state, double frametime);
        void endstep(Gamestate *state, double frametime);
        std::string getsprite(Gamestate *state, bool mask) {return "projectiles/bullet.png";}
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new PeacemakerBullet(*this));}
        void render(Renderer *renderer, Gamestate *state);
        AnimationState* animstate() override {return &animstate_;}
    protected:
    private:
        AnimationState animstate_;
};

#endif // PEACEMAKERBULLET_H
