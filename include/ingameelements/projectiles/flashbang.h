#ifndef FLASHBANG_H
#define FLASHBANG_H

#include <ingameelements/projectile.h>


class Flashbang : public Projectile
{
    public:
        Flashbang(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~Flashbang();
        void beginstep(Gamestate *state, double frametime) {}
        void midstep(Gamestate *state, double frametime);
        std::string getsprite(Gamestate *state, bool mask) {return "heroes/mccree/projectiles/flashbang";}
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Flashbang(*this));}
        bool isrectangular() {return true;}
        Rect getrect() {return Rect(x-2, y-4, 3, 5);}
        double getradius() {return 0;}
        double damage() {return 0;}
        void render(Renderer *renderer, Gamestate *state);
        void explode(Gamestate *state);

        Timer countdown;
    protected:
    private:
};

#endif // FLASHBANG_H
