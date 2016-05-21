#include <list>

#include "entity.h"

Entity::Entity(std::list<Entity*> *gameentities)
{
    gameentities->push_back(this);
}
