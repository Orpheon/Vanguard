#ifndef SERVERNETWORKER_H
#define SERVERNETWORKER_H

#include "networking/networker.h"
#include <vector>


class ServerNetworker : public Networker
{
    public:
        ServerNetworker();
        virtual ~ServerNetworker();
        void receive(Gamestate *state) override;
        void sendeventdata(Gamestate *state) override;
        void sendframedata(Gamestate *state) override;
    protected:
    private:
        int findpeer(Gamestate *state, ENetPeer *peer);
        Player* findplayer(Gamestate *state, ENetPeer *peer);
};

#endif // SERVERNETWORKER_H
