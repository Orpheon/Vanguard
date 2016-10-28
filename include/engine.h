#ifndef ENGINE_H
#define ENGINE_H

#include <string.h>

#include "gamestate.h"
#include "datastructures.h"
#include "spriteloader.h"

class Engine
{
    public:
        Engine(bool isserver_);
        ~Engine();
        void update(WriteBuffer *sendbuffer, double frametime);
        void loadmap(std::string mapname);
        void setinput(EntityPtr myself, InputContainer pressed_keys, InputContainer held_keys, double mouse_x, double mouse_y);

        std::unique_ptr<Gamestate> currentstate;
        std::unique_ptr<Gamestate> oldstate;
        Spriteloader maskloader;
        bool isserver;

        // Make engine move-assigneable, so that " = " doesn't copy but move.
        Engine & operator=(Engine &&)=default;
};

#endif // ENGINE_H
