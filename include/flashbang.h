#ifndef FLASHBANG_H
#define FLASHBANG_H

#include <projectile.h>


class Flashbang : public Projectile
{
    public:
        Flashbang(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~Flashbang();
        void endstep(Gamestate *state, double frametime);
        std::string getsprite(Gamestate *state, bool mask) {return "heroes/mccree/projectiles/flashbang.png";}
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Flashbang(*this));}
        void render(Renderer *renderer, Gamestate *state);
        void explode(Gamestate *state);

        Timer countdown;
    protected:
    private:
};

#endif // FLASHBANG_H
