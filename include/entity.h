#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <allegro5/allegro.h>
#include <string>

// Circular dependency
class Gamestate;
class Renderer;

class Entity
{
    public:
        Entity() = default;
        virtual ~Entity() = default;
        virtual void beginstep(Gamestate *state, double frametime) = 0;
        virtual void midstep(Gamestate *state, double frametime) = 0;
        virtual void endstep(Gamestate *state, double frametime) = 0;
        virtual void render(Renderer *renderer) = 0;
        std::string sprite;
        uint64_t id;
};

#endif // ENTITY_H
