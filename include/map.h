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
    protected:
    private:
        ALLEGRO_BITMAP *background;
        ALLEGRO_BITMAP *wallmask;
};

#endif // MAP_H
