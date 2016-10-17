#include "eventhandler.h"

#include "gamestate.h"

EventHandler::EventHandler()
{
    //ctor
}

EventHandler::~EventHandler()
{
    //dtor
}

void EventHandler::deserialize_event(Gamestate *state, ReadBuffer *buffer)
{
    int eventtype = buffer->read<uint8_t>();
    switch (eventtype)
    {
        case SERVER_SNAPSHOTUPDATE:
            deserialize_server_snapshotupdate(state, buffer);
            break;

        case SERVER_FULLUPDATE:
            deserialize_server_fullupdate(state, buffer);
            break;

       case PLAYER_JOINED:
            deserialize_player_joined(state, buffer);
            break;

        case PLAYER_LEFT:
            deserialize_player_left(state, buffer);
            break;

//        case PRIMARY_FIRED:
//            deserialize_primary_fired(state, buffer);
//            break;
//
//        case SECONDARY_FIRED:
//            deserialize_secondary_fired(state, buffer);
//            break;
//
//        case ABILITY1_USED:
//            deserialize_ability1_used(state, buffer);
//            break;
//
//        case ABILITY2_USED:
//            deserialize_ability2_used(state, buffer);
//            break;
//
//        case ULTIMATE_USED:
//            deserialize_ultimate_used(state, buffer);
//            break;

        default:
            printf("ERROR: Invalid error type received in deserialization: %i", eventtype);
            throw -1;
    }
}


void EventHandler::deserialize_server_snapshotupdate(Gamestate *state, ReadBuffer *buffer)
{
    state->deserializesnapshot(buffer);
}

void EventHandler::deserialize_server_fullupdate(Gamestate *state, ReadBuffer *buffer)
{
    state->deserializefull(buffer);
}

void EventHandler::deserialize_player_joined(Gamestate *state, ReadBuffer *buffer)
{
    state->addplayer();
}

void EventHandler::deserialize_player_left(Gamestate *state, ReadBuffer *buffer)
{
    int playerid = buffer->read<uint8_t>();
    state->removeplayer(playerid);
}
