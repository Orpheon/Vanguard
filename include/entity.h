#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <allegro5/allegro.h>
#include <string>

// Circular dependency
class Gamestate;

class Entity
{
    public:
        Entity() = default;
        virtual ~Entity() = default;
        virtual void beginstep(Gamestate *state, double frametime) = 0;
        virtual void midstep(Gamestate *state, double frametime) = 0;
        virtual void endstep(Gamestate *state, double frametime) = 0;
        virtual void render(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *midground, ALLEGRO_BITMAP *foreground, int cam_x, int cam_y) = 0;
        virtual void clone(Gamestate *oldstate, Gamestate *newstate) = 0;
        std::string sprite;
};

#endif // ENTITY_H
