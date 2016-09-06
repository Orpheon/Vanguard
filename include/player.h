#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "datastructures.h"

// Circular dependency
class Gamestate;

class Player
{
    public:
        Player(Gamestate *state);
        virtual ~Player();
        void midstep(Gamestate *state, double frametime);
        void clone(Gamestate *oldstate, Gamestate *newstate);
        void spawn(Gamestate *state, double x, double y);

        EntityPtr character;
        uint64_t id;
};

#endif // PLAYER_H
