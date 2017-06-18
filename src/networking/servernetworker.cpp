#include "networking/servernetworker.h"
#include "networking/uuid.h"
#include "global_constants.h"
#include "global.h"

#include <cstdint>

ServerNetworker::ServerNetworker(WriteBuffer &sendbuffer_) : Networker(true, sendbuffer_)
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 3224; // 3223-3230
    host = enet_host_create(&address, PLAYER_LIMIT, 1, 0, 0);
    if (host == NULL)
    {
        Global::logging().panic(__FILE__, __LINE__, "Failed to create server host");
    }

    lobbyreminder.init(30, std::bind(&ServerNetworker::registerlobby, this, std::placeholders::_1));
    lobbyreminder.timer = lobbyreminder.duration; // Fire immediately
    if (Global::settings()["Display on Lobby"])
    {
        ENetAddress lobbyaddress;
        enet_address_set_host(&lobbyaddress, LOBBY_HOST);
        lobbyaddress.port = 29944;
        lobby = enet_host_connect(host, &lobbyaddress, 1, 0);
        if (lobby == NULL)
        {
            Global::logging().panic(__FILE__, __LINE__, "Failed to create lobby host");
        }
    }
    else
    {
        lobbyreminder.active = false;
        lobby = nullptr;
    }

    serverid = xg::newUuid();
}

ServerNetworker::~ServerNetworker()
{

}

void ServerNetworker::receive(Gamestate &state)
{
    ENetEvent event;
    while (enet_host_service(host, &event, 0))
    {
        if (event.type == ENET_EVENT_TYPE_CONNECT)
        {
            EntityPtr player = state.addplayer();
            event.peer->data = std::malloc(sizeof(int));
            reinterpret_cast<int*>(event.peer->data)[0] = state.playerlist[state.playerlist.size()-1];

            enet_peer_timeout(event.peer, 0, 2000, 3000);

            // Send full update
            WriteBuffer frame = WriteBuffer();
            frame.write<uint8_t>(SERVER_FULLUPDATE);
            state.serializefull(frame);
            ENetPacket *eventpacket = enet_packet_create(frame.getdata(), frame.length(), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(event.peer, 0, eventpacket);

            // Set the spawn timer for the new player so that they will spawn at next opportunity
            state.get<Player>(player).spawntimer.timer = state.get<Player>(player).spawntimer.duration;
            state.get<Player>(player).spawntimer.active = true;

            // Tell everyone except the new player that a new player joined
            WriteBuffer tmpbuffer;
            tmpbuffer.write<uint8_t>(PLAYER_JOINED);
            eventpacket = enet_packet_create(tmpbuffer.getdata(), tmpbuffer.length(), ENET_PACKET_FLAG_RELIABLE);
            for (unsigned int i=0; i<host->connectedPeers; ++i)
            {
                if (&(host->peers[i]) != event.peer)
                {
                    enet_peer_send(&(host->peers[i]), 0, eventpacket);
                }
            }

            enet_host_flush(host);
        }
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
        {
            int i = state.findplayerid(reinterpret_cast<int*>(event.peer->data)[0]);
            state.removeplayer(i);
            std::free(event.peer->data);

            // Send disconnect event of that player
            sendbuffer.write<uint8_t>(PLAYER_LEFT);
            sendbuffer.write<uint8_t>(i);
        }
        else if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            ReadBuffer data = ReadBuffer(event.packet->data, event.packet->dataLength);
            while (data.length() > 0)
            {
                int eventtype = data.read<uint8_t>();
                Player &player = state.get<Player>(reinterpret_cast<int*>(event.peer->data)[0]);
                if (eventtype == CLIENT_INPUT)
                {
                    InputContainer heldkeys;
                    heldkeys.deserialize(data);
                    double mouse_x = data.read<int16_t>();
                    double mouse_y = data.read<int16_t>();
                    if (state.exists(player.character))
                    {
                        player.getcharacter(state).setinput(state, heldkeys, mouse_x, mouse_y);
                    }
                }
                else if (eventtype == PLAYER_CHANGECLASS)
                {
                    Heroclass newclass = static_cast<Heroclass>(data.read<uint8_t>());
                    player.changeclass(state, newclass);
                    sendbuffer.write<uint8_t>(PLAYER_CHANGECLASS);
                    sendbuffer.write<uint8_t>(state.findplayerid(player.id));
                    sendbuffer.write<uint8_t>(static_cast<uint8_t>(newclass));
                }
                else
                {
                    Global::logging().print(__FILE__, __LINE__, "Invalid packet received on server from player %i: %i",
                                            state.findplayerid(player.id), eventtype);
                }
            }
            enet_packet_destroy(event.packet);
        }
    }
}

void ServerNetworker::sendeventdata(Gamestate &state)
{
    if (sendbuffer.length() > 0)
    {
        ENetPacket *eventpacket = enet_packet_create(sendbuffer.getdata(), sendbuffer.length(), ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(host, 0, eventpacket);
        enet_host_flush(host);
        sendbuffer.reset();
    }
}

void ServerNetworker::sendframedata(Gamestate &state)
{
    WriteBuffer frame = WriteBuffer();
    frame.write<uint8_t>(SERVER_SNAPSHOTUPDATE);
    state.serializesnapshot(frame);
    ENetPacket *framepacket = enet_packet_create(frame.getdata(), frame.length(), 0);
    enet_host_broadcast(host, 0, framepacket);
    enet_host_flush(host);

    if (not lobbyreminder.active and Global::settings()["Display on Lobby"])
    {
        lobbyreminder.reset();
    }
    lobbyreminder.update(state, NETWORKING_TIMESTEP);
}

void ServerNetworker::registerlobby(Gamestate &state)
{
    xg::Uuid message_type(LOBBY_MESSAGE_TYPE_REGISTER);
    xg::Uuid lobbyid(GG2_IDENTIFIER);
    xg::Uuid compatibility(COMPATIBILITY_IDENTIFIER);

    WriteBuffer buffer;

    // See https://github.com/Medo42/Faucet-Lobby/blob/master/Protocol%20Spec.txt for reference
    // https://github.com/PyGG2/PyGG2/blob/master/server/lobby.py is also helpful

    // Message type for lobby
    for (auto& byte : message_type._bytes)
    {
        buffer.write<uint8_t>(byte);
    }
    // Server id
    for (auto& byte : serverid._bytes)
    {
        buffer.write<uint8_t>(byte);
    }
    // Lobby id
    for (auto& byte : lobbyid._bytes)
    {
        buffer.write<uint8_t>(byte);
    }
    // Transport protocol (0=TCP, 1=UDP)
    buffer.write<uint8_t>(1);
    // Port number
    buffer.write<uint16_t>(host->address.port);
    // Number of total player slots
    buffer.write<uint16_t>(PLAYER_LIMIT);
    // Number of players ingame
    buffer.write<uint16_t>(static_cast<uint16_t>(state.playerlist.size()));
    // Number of AI bots
    buffer.write<uint16_t>(0);
    // Flags
    buffer.write<uint16_t>(0);

    std::map<std::string, std::string> data = {
        {"name", "Vanguard test server"},
        {"game", GAME_NAME},
        {"game_uint16_t", GAME_NAME_SHORT},
        {"game_ver", GAME_VERSION},
        {"game_url", GAME_URL},
        {"map", state.currentmap->name}
    };

    // Extra data

    // Write number of key/value pairs
    // Protocol_id isn't a string but fixed so doing it separately
    buffer.write<uint16_t>(data.size() + 1);

    // Compatibility flag
    buffer.write<uint8_t>(std::string("protocol_id").size());
    buffer.writestring("protocol_id");
    buffer.write<uint16_t>(16);
    for (auto& byte : compatibility._bytes)
    {
        buffer.write<uint8_t>(byte);
    }

    // Everything else
    for (auto& entry : data)
    {
        buffer.write<uint8_t>(entry.first.size());
        buffer.writestring(entry.first);
        buffer.write<uint16_t>(entry.second.size());
        buffer.writestring(entry.second);
    }

    ENetPacket *lobbypacket = enet_packet_create(buffer.getdata(), buffer.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(lobby, 0, lobbypacket);
}