#ifndef CORPSE_H
#define CORPSE_H

#include <ingameelements/movingentity.h>
#include "timer.h"

class Corpse : public MovingEntity
{
    public:
        Corpse(uint64_t id_, Gamestate *state, std::string characterfolder, bool isflipped_);
        virtual ~Corpse();
        void beginstep(Gamestate *state, double frametime) {}
        void midstep(Gamestate *state, double frametime);
        void endstep(Gamestate *state, double frametime) {}
        void render(Renderer *renderer, Gamestate *state);
        bool isrootobject() {return true;};
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Corpse(*this));}
        void interpolate(Entity *prev_entity, Entity *next_entity, double alpha);
        std::string getsprite(Gamestate *state, bool mask) {return spritepath;}
    protected:
    private:
        std::string spritepath;
        Timer countdown;
        bool isflipped;
};

#endif // CORPSE_H
