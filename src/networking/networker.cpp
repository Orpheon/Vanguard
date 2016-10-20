#include "networking/networker.h"
#include "global_constants.h"

Networker::Networker(bool isserver) : sendbuffer()
{
    //ctor
}

Networker::~Networker()
{
    enet_host_destroy(host);
}
