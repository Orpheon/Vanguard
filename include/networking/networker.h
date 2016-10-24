#ifndef NETWORKER_H
#define NETWORKER_H

#include "gamestate.h"
#define boolean enet_boolean
#include <enet/enet.h>
#undef enet_boolean

class Networker
{
    public:
        Networker(bool isserver);
        virtual ~Networker();
        virtual void receive(Gamestate *state) = 0;
        virtual void sendeventdata(Gamestate *state) = 0;
        WriteBuffer sendbuffer;
    protected:
        ENetHost *host;
    private:
};

#endif // NETWORKER_H
