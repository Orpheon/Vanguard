#ifndef ENTITY_H
#define ENTITY_H

#include "engine.h"
#include <list>

class Entity
{
    public:
        virtual void beginstep(Engine engine, double frametime) = 0;
        virtual void midstep(Engine engine, double frametime) = 0;
        virtual void endstep(Engine engine, double frametime) = 0;
        virtual void render() = 0;
        virtual Entity* clone() = 0;
};

#endif // ENTITY_H
