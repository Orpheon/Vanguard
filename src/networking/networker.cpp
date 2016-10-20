#include "networking/networker.h"
#include "global_constants.h"

Networker::Networker(bool isserver) : sendbuffer()
{
    if (isserver)
    {
        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = 3224; // 3223-3230
        host = enet_host_create(&address, PLAYER_LIMIT, 2, 0, 0);
    }
    else
    {
        host = enet_host_create(NULL, 1, 2, 0, 0);
    }
}

Networker::~Networker()
{
    enet_host_destroy(host);
}
