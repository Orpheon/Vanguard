#ifndef ENTITY_H
#define ENTITY_H

#include "engine.h"
#include <list>

class Entity
{
    public:
        virtual void beginstep(Gamestate *state, double frametime) = 0;
        virtual void midstep(Gamestate *state, double frametime) = 0;
        virtual void endstep(Gamestate *state, double frametime) = 0;
        virtual void render() = 0;
        virtual Entity* clone() = 0;
};

#endif // ENTITY_H
