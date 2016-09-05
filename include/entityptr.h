#ifndef ENTITYPTR_H_INCLUDED
#define ENTITYPTR_H_INCLUDED

#include "player.h"

template<class EntityT>
struct EntityPtr
{
    EntityPtr(uint64_t entityid_) : entityid(entityid_)
    {
        ;
    }

    // This error is caused by gamestate not being defined. The circular reference is thorny to undo, though - there needs to be a better solution.
    EntityT* get(Gamestate* gs) const
    {
        return static_cast<EntityT*>(gs->entitylist[entityid]);
    }

    uint64_t entityid;
};

struct PlayerPtr
{
    PlayerPtr(uint64_t playerid_) : playerid(playerid_)
    {
        ;
    }

    Player* get(Gamestate* gs) const
    {
        return static_cast<Player*>gs->playerlist[playerid];
    }

    uint64_t playerid;
};

#endif // ENTITYPTR_H_INCLUDED
