#include "networking/clientnetworker.h"

ClientNetworker::ClientNetworker() : Networker(false), connected(false)
{
    ENetAddress serveraddress;
    enet_address_set_host(&serveraddress, "127.0.0.1");
    serveraddress.port = 3224;
    host = enet_host_create(NULL, 1, 1, 0, 0);
    server = enet_host_connect(host, &serveraddress, 1, 0);
}

ClientNetworker::~ClientNetworker()
{
    //dtor
}

void ClientNetworker::receive(Gamestate *state)
{
    ENetEvent event;
    while (enet_host_service(host, &event, 0))
    {
        if (event.type == ENET_EVENT_TYPE_CONNECT)
        {
            ;
        }
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
        {
            printf("\nDisconnected.");
            throw 0;
        }
        else if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            printf("\nClient received packet of length %i", event.packet->dataLength);
            ReadBuffer data = ReadBuffer(event.packet->data, event.packet->dataLength);
            while (data.length() > 0)
            {
                int eventtype = data.read<uint8_t>();
                printf("\nEvent type: %i", eventtype);
                if (eventtype == SERVER_FULLUPDATE)
                {
                    state->deserializefull(&data);
                    connected = true;
                }
                else if (eventtype == SERVER_SNAPSHOTUPDATE)
                {
                    state->deserializesnapshot(&data);
                }
                else if (eventtype == PLAYER_JOINED)
                {
                    state->addplayer();
                }
                else if (eventtype == PLAYER_LEFT)
                {
                    int playerid = data.read<uint8_t>();
                    state->removeplayer(playerid);
                }
                else if (eventtype == PLAYER_SPAWNED)
                {
                    int playerid = data.read<uint8_t>();
                    state->findplayer(playerid)->spawn(state);
                }
                else
                {
                    fprintf(stderr, "\nInvalid packet received on client: %i!", eventtype);
                }
            }
            enet_packet_destroy(event.packet);
        }
    }
}

void ClientNetworker::sendeventdata(Gamestate *state)
{
    if (sendbuffer.length() > 0)
    {
        ENetPacket *eventpacket = enet_packet_create(sendbuffer.getdata(), sendbuffer.length(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(server, 0, eventpacket);
        enet_host_flush (host);
        sendbuffer.reset();
    }
}

void ClientNetworker::sendinput(INPUT_CONTAINER pressedkeys, INPUT_CONTAINER heldkeys, float mouse_x, float mouse_y)
{
    WriteBuffer input = WriteBuffer();
    input.write<uint8_t>(CLIENT_INPUT);
    pressedkeys.serialize(&input);
    heldkeys.serialize(&input);
    input.write<float>(mouse_x);
    input.write<float>(mouse_y);
    ENetPacket *inputpacket = enet_packet_create(input.getdata(), input.length(), 0);
    enet_host_broadcast(host, 0, inputpacket);
    enet_host_flush(host);
}
