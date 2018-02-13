#pragma once

#include <string.h>

#include "gamestate.h"
#include "datastructures.h"
#include "spriteloader.h"

class Engine
{
    public:
        Engine(bool isserver_);
        ~Engine();
        void update(double frametime);
        void nextmap();
        void loadrotation(std::vector<std::string> &mapnames);

        std::unique_ptr<Gamestate> currentstate;
        std::unique_ptr<Gamestate> oldstate;
        Spriteloader maskloader;
        bool isserver;
        WriteBuffer sendbuffer;
        unsigned long maprotationindex;
        std::vector<std::string> maprotation;

        // Make engine move-assigneable, so that " = " doesn't copy but move.
        Engine & operator=(Engine &&)=default;
};

