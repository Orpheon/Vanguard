#include "entity.h"

#include <iostream>

void Entity::init(uint64_t id_)
{
    inited = true;
    id = id_;
}

void Entity::beginstep(Gamestate *state, double frametime)
{
    if (not inited)
    {
        std::cout << "Fatal error: Entity of type " << static_cast<int>(entitytype) << " beginstep was called before init function!";
        throw -1;
    }
}

void Entity::midstep(Gamestate *state, double frametime)
{
    if (not inited)
    {
        std::cout << "Fatal error: Entity of type " << static_cast<int>(entitytype) << " midstep was called before init function!";
        throw -1;
    }
}

void Entity::endstep(Gamestate *state, double frametime)
{
    if (not inited)
    {
        std::cout << "Fatal error: Entity of type " << static_cast<int>(entitytype) << " endstep was called before init function!";
        throw -1;
    }
}

void Entity::render(Renderer *renderer, Gamestate *state)
{
    if (not inited)
    {
        std::cout << "Fatal error: Entity of type " << static_cast<int>(entitytype) << " render was called before init function!";
        throw -1;
    }
}
