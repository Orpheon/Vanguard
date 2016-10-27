#ifndef PEACEMAKERBULLET_H
#define PEACEMAKERBULLET_H

#include "ingameelements/projectile.h"
#include <string>

class PeacemakerBullet : public Projectile
{
    public:
        PeacemakerBullet(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~PeacemakerBullet();
        void beginstep(Gamestate *state, double frametime) {}
        std::string getsprite(Gamestate *state, bool mask) {return "heroes/mccree/projectiles/bullet";}
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new PeacemakerBullet(*this));}
        bool isrectangular() {return true;}
        Rect getrect() {return Rect(x, y, 20, 1);}
        double getradius() {return 0;}
        void render(Renderer *renderer, Gamestate *state);
        double damage() {return 70;}
    protected:
    private:
};

#endif // PEACEMAKERBULLET_H
