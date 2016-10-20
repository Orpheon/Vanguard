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
        void send(Gamestate *state) override;
    protected:
    private:
        std::vector<ENetPeer*> clients;
        int findpeer(ENetPeer *peer);
};

#endif // SERVERNETWORKER_H
