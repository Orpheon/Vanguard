#ifndef RENDERER_H
#define RENDERER_H

#include "gamestate.h"
#include "spriteloader.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

class Renderer
{
    public:
        Renderer();
        ~Renderer();
        void render(Gamestate *currentstate, EntityPtr myself);
        ALLEGRO_DISPLAY *display;
        double cam_x;
        double cam_y;
        ALLEGRO_BITMAP *background;
        ALLEGRO_BITMAP *midground;
        ALLEGRO_BITMAP *foreground;
        Spriteloader spriteloader;
        ALLEGRO_FONT *font;
    protected:
    private:
        double lasttime;
        double currenttime;
        double frametime;
        double deltatime;
        double fps;
};

#endif // RENDERER_H
