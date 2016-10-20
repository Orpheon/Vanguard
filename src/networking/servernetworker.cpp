#include "networking/servernetworker.h"
#include "global_constants.h"

ServerNetworker::ServerNetworker() : Networker(true)
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 3224; // 3223-3230
    host = enet_host_create(&address, PLAYER_LIMIT, 2, 0, 0);
}

ServerNetworker::~ServerNetworker()
{
    //dtor
}

void ServerNetworker::receive(Gamestate *state)
{
    ENetEvent event;
    while (enet_host_service(host, &event, 0))
    {
        if (event.type == ENET_EVENT_TYPE_CONNECT)
        {
            state->addplayer(event.peer);
        }
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
        {
            int i = findpeer(state, event.peer);
            state->removeplayer(i);
            // TODO: Send PLAYER_DISCONNECT event
        }
        else if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            ReadBuffer data = ReadBuffer(event.packet->data, event.packet->dataLength);
            int eventtype = data.read<uint8_t>();
            enet_packet_destroy(event.packet);
            if (eventtype == CLIENT_INPUT)
            {
                Player *p = findplayer(state, event.peer);
                if (p->character != 0)
                {
                    INPUT_CONTAINER pressedkeys = INPUT_CONTAINER();
                    INPUT_CONTAINER heldkeys = INPUT_CONTAINER();
                    pressedkeys.deserialize(&data);
                    heldkeys.deserialize(&data);
                    double mouse_x = data.read<float>();
                    double mouse_y = data.read<float>();
                    p->getcharacter(state)->setinput(state, pressedkeys, heldkeys, mouse_x, mouse_y);
                }
            }
            else
            {
                fprintf(stderr, "Invalid packet received on server: %i!", eventtype);
            }
        }
    }
}

void ServerNetworker::sendeventdata(Gamestate *state)
{
    if (sendbuffer.length() > 0)
    {
        ENetPacket *eventpacket = enet_packet_create(sendbuffer.getdata(), sendbuffer.length(), ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(host, 1, eventpacket);
        enet_host_flush(host);
    }
}

void ServerNetworker::sendframedata(Gamestate *state)
{
    WriteBuffer frame = WriteBuffer();
    state->serializesnapshot(&frame);
    ENetPacket *framepacket = enet_packet_create(frame.getdata(), frame.length(), 0);
    enet_host_broadcast(host, 0, framepacket);
    enet_host_flush(host);
}

int ServerNetworker::findpeer(Gamestate *state, ENetPeer *peer)
{
    for (unsigned int i=0; i<state->playerlist.size(); ++i)
    {
        if (state->get<Player>(state->playerlist[i])->networkconnection == peer)
        {
            return i;
        }
    }
    fprintf(stderr, "Searched for nonexistent player!");
    throw -1;
}

Player* ServerNetworker::findplayer(Gamestate *state, ENetPeer *peer)
{
    Player *p;
    for (unsigned int i=0; i<state->playerlist.size(); ++i)
    {
        p = state->get<Player>(state->playerlist[i]);
        if (p->networkconnection == peer)
        {
            return p;
        }
    }
    fprintf(stderr, "Searched for nonexistent player!");
    throw -1;
}
