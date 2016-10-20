#ifndef NETWORKER_H
#define NETWORKER_H

#include "gamestate.h"
#include <enet/enet.h>

class Networker
{
    public:
        Networker(bool isserver);
        virtual ~Networker();
        virtual void receive(Gamestate *state) = 0;
        virtual void sendeventdata(Gamestate *state) = 0;
        virtual void sendframedata(Gamestate *state) = 0;
        WriteBuffer sendbuffer;
    protected:
        ENetHost *host;
    private:
};

#endif // NETWORKER_H
