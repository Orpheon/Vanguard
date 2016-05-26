#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

#include "entity.h"

class Gamestate
{
    public:
        Gamestate();
        ~Gamestate();
        void update(double frametime);
        Gamestate* clone();

        std::vector<Entity*> entitylist;
        double time;
    protected:
    private:
};

#endif // GAMESTATE_H
