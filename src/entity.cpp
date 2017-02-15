#include "entity.h"

#include <iostream>

void Entity::init(uint64_t id_)
{
    inited = true;
    id = id_;
}
