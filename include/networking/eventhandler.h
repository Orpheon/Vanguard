#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "datastructures.h"

class EventHandler
{
    public:
        EventHandler();
        virtual ~EventHandler();
        void deserialize_event(Gamestate *state, ReadBuffer *buffer, int eventtype);
    protected:
        void deserialize_server_snapshotupdate(Gamestate *state, ReadBuffer *buffer);
        void deserialize_server_fullupdate(Gamestate *state, ReadBuffer *buffer);
        void deserialize_player_joined(Gamestate *state, ReadBuffer *buffer);
        void deserialize_player_left(Gamestate *state, ReadBuffer *buffer);
//        void deserialize_primary_fired(Gamestate *state, ReadBuffer *buffer);
//        void deserialize_secondary_fired(Gamestate *state, ReadBuffer *buffer);
//        void deserialize_ability1_used(Gamestate *state, ReadBuffer *buffer);
//        void deserialize_ability2_used(Gamestate *state, ReadBuffer *buffer);
//        void deserialize_ultimate_used(Gamestate *state, ReadBuffer *buffer);
    private:
};

#endif // EVENTHANDLER_H
