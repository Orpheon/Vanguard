#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "gamestate.h"

// FIXME: Circular dependencies between player.h and gamestate.h (as well as indirect character.h)
class Character;

class Player
{
    public:
        Player(Gamestate *state);
        virtual ~Player();
        void midstep(Gamestate *state, double frametime);
        std::unique_ptr<Player> clone(Gamestate *state);
        void spawn(Gamestate *state, double x, double y);

        int characterid;
};

#endif // PLAYER_H
