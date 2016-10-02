#ifndef PLAYER_H
#define PLAYER_H

#include <memory>

#include "entity.h"
#include "datastructures.h"

// Circular dependency
class Gamestate;
class Character;

class Player : public Entity
{
    public:
        Player(Gamestate *state);
        virtual ~Player();
        void beginstep(Gamestate *state, double frametime);
        void midstep(Gamestate *state, double frametime);
        void endstep(Gamestate *state, double frametime);
        void clone(Gamestate *oldstate, Gamestate *newstate);
        void spawn(Gamestate *state, double x, double y);
        bool isrootobject() {return true;}
        void render(Renderer *renderer, Gamestate *state);
        std::string getsprite(Gamestate *state, bool mask) {}

        Character* getcharacter(Gamestate *state);

        EntityPtr character;
        uint64_t id;
};

#endif // PLAYER_H
