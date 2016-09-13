#ifndef ENGINE_H
#define ENGINE_H

#include <string.h>

#include "gamestate.h"
#include "datastructures.h"

class Engine
{
    public:
        Engine();
        ~Engine();
        void run();
        void loadmap(std::string mapname);
        PlayerPtr newplayer();

        Gamestate currentstate;

    private:
        long int getmillisec();

		long int lasttimeupdated;
		double millisecperframe;
};

#endif // ENGINE_H
