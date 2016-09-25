#ifndef MAP_H
#define MAP_H

#include "datastructures.h"
#include "movingentity.h"

#include <allegro5/allegro.h>
#include <string>

class Map
{
    public:
        Map(std::string name);
        ~Map();
        void render(double cam_x, double cam_y);
        bool collides(Gamestate *state, MovingEntity *entity);
        bool collides(Gamestate *state, Rect rect);
        bool place_free(double x, double y) {return al_get_pixel(wallmask, x, y).a == 0;}
    protected:
    private:
        ALLEGRO_BITMAP *background;
        ALLEGRO_BITMAP *wallmask;
};

#endif // MAP_H
