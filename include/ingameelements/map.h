#ifndef MAP_H
#define MAP_H

#include "datastructures.h"
#include "ingameelements/movingentity.h"
#include "ingameelements/character.h"
#include "json.hpp"

#include <allegro5/allegro.h>
#include <string>

class Map
{
    public:
        Map(Gamestate *state, std::string name);
        ~Map();
        void renderbackground(double cam_x, double cam_y);
        void renderwallground(double cam_x, double cam_y);
        bool collides(Gamestate *state, MovingEntity *entity);
        bool collides(Gamestate *state, Character *entity);
        bool collides(Rect rect);
        bool collides(double rotx, double roty, Rect rect, double angle);
        bool collideline(double x1, double y1, double x2, double y2);
        bool testpixel(double x, double y) {return x < 0 or y < 0 or x > al_get_bitmap_width(wallmask) or y > al_get_bitmap_height(wallmask) or al_get_pixel(wallmask, x, y).a != 0;}
        double width() {return al_get_bitmap_width(background);}
        double height() {return al_get_bitmap_height(background);}
    protected:
    private:
        ALLEGRO_BITMAP *background;
        ALLEGRO_BITMAP *wallground;
        ALLEGRO_BITMAP *wallmask;
        nlohmann::json mapdata;
};

#endif // MAP_H
