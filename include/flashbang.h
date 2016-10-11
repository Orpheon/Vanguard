#ifndef FLASHBANG_H
#define FLASHBANG_H

#include <projectile.h>


class Flashbang : public Projectile
{
    public:
        Flashbang(uint64_t id_, Gamestate *state, EntityPtr owner_);
        virtual ~Flashbang();
        void endstep(Gamestate *state, double frametime);
        std::string getsprite(Gamestate *state, bool mask) {return "projectiles/bullet.png";} // Placeholder
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Flashbang(*this));}
        void render(Renderer *renderer, Gamestate *state);
        void explode(Gamestate *state);

        Timer countdown;
        AnimationState* animstate() override {return &animstate_;}
    protected:
    private:
        AnimationState animstate_;
};

#endif // FLASHBANG_H
