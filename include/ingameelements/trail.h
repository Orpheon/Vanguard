#ifndef TRAIL_H
#define TRAIL_H

#include "renderer.h"

class Trail : public Entity
{
    public:
        Trail(uint64_t id_, Gamestate *state, ALLEGRO_COLOR color_, double x1_, double y1_, double x2_, double y2_, double duration);
        virtual ~Trail();
        void beginstep(Gamestate *state, double frametime) {}
        void midstep(Gamestate *state, double frametime);
        void endstep(Gamestate *state, double frametime) {}
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Trail(*this));}
        void render(Renderer *renderer, Gamestate *state);
        void interpolate(Entity *prev_entity, Entity *next_entity, double alpha);
        bool isrootobject() {return true;}

        ALLEGRO_COLOR color;
        double x1;
        double y1;
        double x2;
        double y2;
        Timer countdown;
    protected:
    private:
};

#endif // TRAIL_H
