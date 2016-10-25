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
        Entity(uint64_t id_) : id(id_) {}
        virtual ~Entity() = default;
        virtual void beginstep(Gamestate *state, double frametime) = 0;
        virtual void midstep(Gamestate *state, double frametime) = 0;
        virtual void endstep(Gamestate *state, double frametime) = 0;
        virtual void render(Renderer *renderer, Gamestate *state) = 0;
        virtual bool isrootobject() = 0;
        virtual std::unique_ptr<Entity> clone() = 0;
        virtual void interpolate(Entity *prev_entity, Entity *next_entity, double alpha) = 0;
        virtual void destroy(Gamestate *state) {destroyentity = true;}
        ENTITYTYPE entitytype;
        uint64_t id;
        bool destroyentity = false;
};

#endif // ENTITY_H
