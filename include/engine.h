#ifndef ENGINE_H
#define ENGINE_H

#include <string.h>

#include "gamestate.h"
#include "datastructures.h"
#include "maskloader.h"

class Engine
{
    public:
        Engine();
        ~Engine();
        void run();
        void loadmap(std::string mapname);
        PlayerPtr newplayer();

        Gamestate currentstate;
        Maskloader maskloader;

    private:
        long int getmillisec();

		long int lasttimeupdated;
		double millisecperframe;
};

#endif // ENGINE_H
