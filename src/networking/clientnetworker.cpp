#include "networking/clientnetworker.h"

#include "global.h"
#include "ingameelements/gamemodes/kothmanager.h"
#include "ingameelements/heroes/reinhardt.h"

ClientNetworker::ClientNetworker(WriteBuffer &sendbuffer_, std::string serverip, int serverport)
    : Networker(false, sendbuffer_), connected(false)
{
    ENetAddress serveraddress;
    enet_address_set_host(&serveraddress, serverip.c_str());
    serveraddress.port = static_cast<uint16_t>(serverport);
    host = enet_host_create(NULL, 1, 2, 0, 0);
    if (host == NULL)
    {
        Global::logging().panic(__FILE__, __LINE__, "Failed to create client host");
    }
    server = enet_host_connect(host, &serveraddress, 2, 0);
}

ClientNetworker::~ClientNetworker()
{
    //dtor
}

void ClientNetworker::receive(Gamestate &state)
{
    ENetEvent event;
    while (enet_host_service(host, &event, 0))
    {
        if (event.type == ENET_EVENT_TYPE_CONNECT)
        {
            std::string name = Global::settings()["Player name"];
            sendbuffer.write<uint8_t>(PLAYER_CHANGENAME);
            sendbuffer.write<uint8_t>(name.length());
            sendbuffer.writestring(name);
        }
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT)
        {
            Global::logging().panic(__FILE__, __LINE__, "Disconnected from server");
        }
        else if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            ReadBuffer data = ReadBuffer(event.packet->data, event.packet->dataLength);
            while (data.length() > 0)
            {
                int eventtype = data.read<uint8_t>();
                if (eventtype == SERVER_FULLUPDATE)
                {
                    state.deserializefull(data);
                    connected = true;
                }
                else if (eventtype == SERVER_SNAPSHOTUPDATE)
                {
                    state.deserializesnapshot(data);
                    // Resets gun arm position and a bunch of similar things to be up to date
                    state.update(0);
                }
                else if (eventtype == PLAYER_JOINED)
                {
                    state.addplayer();
                }
                else if (eventtype == PLAYER_LEFT)
                {
                    int playerid = data.read<uint8_t>();
                    state.removeplayer(playerid);
                }
                else if (eventtype == PLAYER_CHANGECLASS)
                {
                    int playerid = data.read<uint8_t>();
                    state.findplayer(playerid).heroclass = (Heroclass)(data.read<uint8_t>());
                }
                else if (eventtype == PLAYER_SPAWNED)
                {
                    int playerid = data.read<uint8_t>();
                    state.findplayer(playerid).spawn(state);
                }
                else if (eventtype == PLAYER_DIED)
                {
                    int playerid = data.read<uint8_t>();
                    state.findplayer(playerid).getcharacter(state).destroy(state);
                }
                else if (eventtype == PRIMARY_FIRED)
                {
                    int playerid = data.read<uint8_t>();
                    state.findplayer(playerid).getcharacter(state).getweapon(state).fireprimary(state);
                }
                else if (eventtype == SECONDARY_FIRED)
                {
                    int playerid = data.read<uint8_t>();
                    state.findplayer(playerid).getcharacter(state).getweapon(state).firesecondary(state);
                }
                else if (eventtype == ABILITY1_USED)
                {
                    int playerid = data.read<uint8_t>();
                    state.findplayer(playerid).getcharacter(state).useability1(state);
                }
                else if (eventtype == ABILITY2_USED)
                {
                    int playerid = data.read<uint8_t>();
                    state.findplayer(playerid).getcharacter(state).useability2(state);
                }
                else if (eventtype == ULTIMATE_USED)
                {
                    int playerid = data.read<uint8_t>();
                    Player &p = state.findplayer(playerid);
                    p.ultcharge.reset();
                    p.ultcharge.active = false;
                    p.getcharacter(state).useultimate(state);
                }
                else if (eventtype == KOTH_CP_CREATED)
                {
                    Team team = static_cast<Team>(data.read<uint8_t>());
                    KothManager &km = static_cast<KothManager&>(state.currentmap->currentgamemode(state));
                    km.createpoint(state, team);
                }
                else if (eventtype == PLAYER_CHANGENAME)
                {
                    int playerid = data.read<uint8_t>();
                    Player &p = state.findplayer(playerid);
                    int namelength = data.read<uint8_t>();
                    std::string name = data.readstring(namelength);
                    p.name = name;
                }
                else if (eventtype == CHARACTER_PINNED)
                {
                    int reinhardtid = data.read<uint8_t>();
                    Player &reinhardtplayer = state.findplayer(reinhardtid);
                    int targetid = data.read<uint8_t>();
                    Player &targetplayer = state.findplayer(targetid);

                    if (state.exists(reinhardtplayer.character) and state.exists(targetplayer.character))
                    {
                        Reinhardt &reinhardt = state.get<Reinhardt&>(reinhardtplayer.character);
                        reinhardt.pintarget = targetplayer.character;
                        targetplayer.getcharacter(state).pinanim.reset();
                    }
                }
                else
                {
                    Global::logging().panic(__FILE__, __LINE__, "Invalid packet received on client: %i", eventtype);
                }
            }
            enet_packet_destroy(event.packet);
        }
    }
}

void ClientNetworker::sendeventdata(Gamestate &state)
{
    if (sendbuffer.length() > 0)
    {
        ENetPacket *eventpacket = enet_packet_create(sendbuffer.getdata(), sendbuffer.length(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(server, 0, eventpacket);
        enet_host_flush (host);
        sendbuffer.reset();
    }
}

void ClientNetworker::sendinput(InputContainer heldkeys, float mouse_x, float mouse_y)
{
    WriteBuffer input = WriteBuffer();
    input.write<uint8_t>(CLIENT_INPUT);
    heldkeys.serialize(input);
    input.write<int16_t>(mouse_x);
    input.write<int16_t>(mouse_y);
    ENetPacket *inputpacket = enet_packet_create(input.getdata(), input.length(), 0);
    enet_host_broadcast(host, 1, inputpacket);
    enet_host_flush(host);
}
