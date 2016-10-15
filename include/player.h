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
        Player(uint64_t id_, Gamestate *state);
        virtual ~Player();
        void beginstep(Gamestate *state, double frametime);
        void midstep(Gamestate *state, double frametime);
        void endstep(Gamestate *state, double frametime);
        void clone(Gamestate *oldstate, Gamestate *newstate);
        void spawn(Gamestate *state);
        bool isrootobject() {return true;}
        bool issynced() {return true;}
        void render(Renderer *renderer, Gamestate *state);
        void interpolate(Entity *prev_entity, Entity *next_entity, double alpha);
        std::unique_ptr<Entity> clone() {return std::unique_ptr<Entity>(new Player(*this));}
        void serialize(Gamestate *state, WriteBuffer *buffer);
        void deserialize(Gamestate *state, ReadBuffer *buffer);

        Character* getcharacter(Gamestate *state);

        EntityPtr character;
};

#endif // PLAYER_H
