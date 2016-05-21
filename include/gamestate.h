#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <list>

#include "engine.h"
#include "entity.h"

class Gamestate
{
    public:
        Gamestate();
        ~Gamestate();
        void update(Engine engine, double frametime);
        Gamestate* clone();

        std::list<Entity*> *entitylist;
        double time;
    protected:
    private:
};

#endif // GAMESTATE_H
