#include "networking/servernetworker.h"

ServerNetworker::ServerNetworker() : Networker(true), clients()
{
    //ctor
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
            clients.push_back(event.peer);
            state->addplayer();
        }
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
        {
            int i = findpeer(event.peer);
            state->removeplayer(i);
            clients.erase(clients.begin() + i);
            // TODO: Send PLAYER_DISCONNECT event
        }
        else if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            ReadBuffer data = ReadBuffer(event.packet->data, event.packet->dataLength);
            enet_packet_destroy(event.packet);
            int packet_event = data.read<uint8_t>();
            if (packet_event == CLIENT_INPUT)
            {
                Player *p = state->findplayer(findpeer(event.peer));
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
                fprintf(stderr, "Invalid packet received on server!");
            }
        }
    }
}

int ServerNetworker::findpeer(ENetPeer *peer)
{
    for (unsigned int i=0; i<clients.size(); ++i)
    {
        if (clients[i] == peer)
        {
            return i;
        }
    }
    fprintf(stderr, "Searched for nonexistent player!");
    throw -1;
}
