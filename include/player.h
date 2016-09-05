#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "gamestate.h"
#include "datastructures.h"

// FIXME: Circular dependencies between player.h and gamestate.h (as well as indirect character.h)
class Character;

class Player
{
    public:
        Player(Gamestate *state);
        virtual ~Player();
        void midstep(Gamestate *state, double frametime);
        void clone(Gamestate *oldstate, Gamestate *newstate);
        void spawn(Gamestate *state, double x, double y);

        EntityPtr character;
};

#endif // PLAYER_H
