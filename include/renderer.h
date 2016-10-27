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
        virtual ~Renderer();
        void render(ALLEGRO_DISPLAY *display, Gamestate *currentstate, EntityPtr myself);
        double cam_x;
        double cam_y;
        double zoom;
        int WINDOW_WIDTH;
        int WINDOW_HEIGHT;
        ALLEGRO_BITMAP *background;
        ALLEGRO_BITMAP *midground;
        ALLEGRO_BITMAP *foreground;
        ALLEGRO_BITMAP *surfaceground;
        Spriteloader spriteloader;
        ALLEGRO_FONT *font20;
        ALLEGRO_FONT *font10;
        ALLEGRO_FONT *font6;
        ALLEGRO_FONT *gg2font;
    protected:
    private:
        double lasttime;
};

#endif // RENDERER_H
