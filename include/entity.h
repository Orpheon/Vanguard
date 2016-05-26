#ifndef ENTITY_H
#define ENTITY_H

#include <vector>

// FIXME: THIS IS TERRIBLE, solution to solve circular dependency between this file and gamestate.h
// This file needs gamestate for the step method declarations, and gamestate needs this file for the entity list declarations
class Gamestate;

class Entity
{
    public:
        virtual ~Entity();
        virtual void beginstep(Gamestate *state, double frametime) = 0;
        virtual void midstep(Gamestate *state, double frametime) = 0;
        virtual void endstep(Gamestate *state, double frametime) = 0;
        virtual void render() = 0;
        virtual Entity* clone() = 0;
};

#endif // ENTITY_H
