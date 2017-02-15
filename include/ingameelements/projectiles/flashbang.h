#ifndef FLASHBANG_H
#define FLASHBANG_H

#include <ingameelements/projectile.h>


class Flashbang : public Projectile
{
    public:
        Flashbang(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~Flashbang() override;
        void beginstep(Gamestate *state, double frametime) override {}
        void midstep(Gamestate *state, double frametime) override;
        std::string getsprite(Gamestate *state, bool mask) override {return "heroes/mccree/projectiles/flashbang";}
        std::unique_ptr<Entity> clone() override {return std::unique_ptr<Entity>(new Flashbang(*this));}
        bool isrectangular() override {return true;}
        Rect getrect() override {return Rect(x-2, y-4, 3, 5);}
        double getradius() override {return 0;}
        double damage() override {return 0;}
        void render(Renderer *renderer, Gamestate *state) override;
        void explode(Gamestate *state);
        void oncollision(Gamestate *state) override {explode(state);}
        void oncollision(Gamestate *state, Character *c) override {explode(state);}

        Timer countdown;
    protected:
    private:
        double EXPLOSION_RADIUS = 40;
};

#endif // FLASHBANG_H
