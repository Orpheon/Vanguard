#ifndef ENGINE_H
#define ENGINE_H

#include "gamestate.h"


class Engine
{
    public:
        Engine();
        ~Engine();
        void run();
        void loadmap(char *mapname);

        Gamestate currentstate; // FIXME: This used to be private, was taken out to be passed to renderer. Possibly very bad idea.


    private:
        long int getmillisec();

		long int lasttimeupdated;
		double millisecperframe;
};

#endif // ENGINE_H
