#pragma once

#include "ingameelements/projectile.h"
#include <string>

class PeacemakerBullet : public Projectile
{
    public:
        virtual ~PeacemakerBullet() override = default;
        void beginstep(Gamestate &state, double frametime) override {}
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new PeacemakerBullet(*this));}
        Rect getrect() override {return Rect(x, y, 20, 1);}
        void render(Renderer &renderer, Gamestate &state) override;
        double damage() override {return 70;}

        std::string spritepath = "heroes/mccree/projectiles/bullet";
    protected:
    private:
};

