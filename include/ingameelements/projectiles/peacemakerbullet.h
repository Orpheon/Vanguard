#pragma once

#include "ingameelements/projectile.h"
#include <string>

class PeacemakerBullet : public Projectile
{
    public:
        virtual ~PeacemakerBullet() override = default;
        void beginstep(Gamestate *state, double frametime) override {}
        std::string getsprite(Gamestate *state, bool mask) override {return "heroes/mccree/projectiles/bullet";}
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new PeacemakerBullet(*this));}
        bool isrectangular() override {return true;}
        Rect getrect() override {return Rect(x, y, 20, 1);}
        double getradius() override {return 0;}
        void render(Renderer *renderer, Gamestate *state) override;
        double damage() override {return 70;}
    protected:
    private:
};

