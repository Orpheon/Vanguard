
#include "entity.h"
#include "gamestate.h"

Entity::Entity(Gamestate *state)
{
    state->entitylist.push_back(this);
}
