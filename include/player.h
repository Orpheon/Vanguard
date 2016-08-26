#ifndef PLAYER_H
#define PLAYER_H

// FIXME: Circular dependencies between player.h and gamestate.h (as well as indirect character.h)
class Character;
class Gamestate;

class Player
{
    public:
        Player(Gamestate *state);
        virtual ~Player();
        void midstep(Gamestate *state, double frametime);
        Player* clone(Gamestate *state);
        void spawn(Gamestate *state, double x, double y);

        Character *character;
};

#endif // PLAYER_H
