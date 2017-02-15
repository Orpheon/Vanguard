#ifndef SERVERNETWORKER_H
#define SERVERNETWORKER_H

#include "networking/networker.h"
#include <vector>


class ServerNetworker : public Networker
{
    public:
        ServerNetworker();
        virtual ~ServerNetworker() override;
        void receive(Gamestate *state) override;
        void sendeventdata(Gamestate *state) override;
        void sendframedata(Gamestate *state);
    protected:
    private:
};

#endif // SERVERNETWORKER_H
