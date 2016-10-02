#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <allegro5/allegro.h>

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
        virtual void render(Renderer *renderer, Gamestate *state) = 0;
        virtual std::string getsprite(Gamestate *state, bool mask) = 0;
        virtual bool isrootobject() = 0;
        uint64_t id;
};

#endif // ENTITY_H
