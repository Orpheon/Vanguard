#ifndef RENDERER_H
#define RENDERER_H

#include "gamestate.h"

#include <allegro5/allegro.h>

class Renderer
{
    public:
        Renderer();
        ~Renderer();
        void render(const Gamestate& currentstate);
        ALLEGRO_DISPLAY *display;
    protected:
    private:
};

#endif // RENDERER_H
