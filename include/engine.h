#ifndef ENGINE_H
#define ENGINE_H

#include "gamestate.h"


class Engine
{
    public:
        Engine();
        ~Engine();
        void run();

    private:
        long int getmillisec();

		long int lasttimeupdated;
		double millisecperframe;
		Gamestate currentstate;
};

#endif // ENGINE_H
