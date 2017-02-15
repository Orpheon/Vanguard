#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <memory>
#include <vector>
#include <allegro5/allegro.h>
#include "networking/buffer.h"
#include "datastructures.h"

// Circular dependency
class Gamestate;
class Renderer;

class Entity
{
    public:
        virtual ~Entity() = default;
        virtual void init(uint64_t id_);
        virtual void beginstep(Gamestate *state, double frametime);
        virtual void midstep(Gamestate *state, double frametime);
        virtual void endstep(Gamestate *state, double frametime);
        virtual void render(Renderer *renderer, Gamestate *state);
        virtual bool isrootobject() = 0;
        virtual std::unique_ptr<Entity> clone() = 0;
        virtual void interpolate(Entity *prev_entity, Entity *next_entity, double alpha) = 0;
        virtual void destroy(Gamestate *state) {destroyentity = true;}
        ENTITYTYPE entitytype;
        uint64_t id = 0;
        bool destroyentity = false;
        bool inited = false;
};

#endif // ENTITY_H
