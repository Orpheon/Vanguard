#ifndef ENGINE_H
#define ENGINE_H

#include <string.h>

#include "gamestate.h"
#include "datastructures.h"
#include "spriteloader.h"

class Engine
{
    public:
        Engine();
        ~Engine();
        void update(double frametime);
        void loadmap(std::string mapname);
        EntityPtr newplayer();

        Gamestate currentstate;
        Spriteloader maskloader;
};

#endif // ENGINE_H
