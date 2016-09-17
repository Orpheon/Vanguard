#ifndef RENDERER_H
#define RENDERER_H

#include "gamestate.h"
#include "spriteloader.h"

#include <allegro5/allegro.h>

class Renderer
{
    public:
        Renderer();
        ~Renderer();
        void render(Gamestate *currentstate, PlayerPtr myself);
        ALLEGRO_DISPLAY *display;
        double cam_x;
        double cam_y;
        ALLEGRO_BITMAP *background;
        ALLEGRO_BITMAP *midground;
        ALLEGRO_BITMAP *foreground;
        Spriteloader spriteloader;
    protected:
    private:
};

#endif // RENDERER_H
