#include "networking/servernetworker.h"
#include "global_constants.h"

ServerNetworker::ServerNetworker() : Networker(true)
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 3224; // 3223-3230
    host = enet_host_create(&address, PLAYER_LIMIT, 1, 0, 0);
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
            state->addplayer();
            event.peer->data = std::malloc(sizeof(int));
            reinterpret_cast<int*>(event.peer->data)[0] = state->playerlist.size()-1;

            // Send full update
            WriteBuffer frame = WriteBuffer();
            frame.write<uint8_t>(SERVER_FULLUPDATE);
            state->serializefull(&frame);
            printf("\nServer frame length: %i", frame.length()); printf("\n");
            ENetPacket *eventpacket = enet_packet_create(frame.getdata(), frame.length(), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(event.peer, 0, eventpacket);
            enet_host_flush(host);

            // Send spawn event of that player
            sendbuffer.write<uint8_t>(PLAYER_SPAWNED);
            sendbuffer.write<uint8_t>(state->playerlist.size()-1);
            // and spawn said player ourselves
            state->findplayer(state->playerlist.size()-1)->spawn(state);
        }
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
        {
            int i = reinterpret_cast<int*>(event.peer->data)[0];
            state->removeplayer(i);
            std::free(event.peer->data);
            // TODO: Send PLAYER_DISCONNECT event
        }
        else if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            ReadBuffer data = ReadBuffer(event.packet->data, event.packet->dataLength);
            while (data.length() > 0)
            {
                int eventtype = data.read<uint8_t>();
                if (eventtype == CLIENT_INPUT)
                {
                    Player *p = state->findplayer(reinterpret_cast<int*>(event.peer->data)[0]);
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
            enet_packet_destroy(event.packet);
        }
    }
}

void ServerNetworker::sendeventdata(Gamestate *state)
{
    if (sendbuffer.length() > 0)
    {
        ENetPacket *eventpacket = enet_packet_create(sendbuffer.getdata(), sendbuffer.length(), ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(host, 0, eventpacket);
        enet_host_flush(host);
        sendbuffer.reset();
    }
}

void ServerNetworker::sendframedata(Gamestate *state)
{
    WriteBuffer frame = WriteBuffer();
    frame.write<uint8_t>(SERVER_SNAPSHOTUPDATE);
    state->serializesnapshot(&frame);
    ENetPacket *framepacket = enet_packet_create(frame.getdata(), frame.length(), 0);
    enet_host_broadcast(host, 0, framepacket);
    enet_host_flush(host);
}
