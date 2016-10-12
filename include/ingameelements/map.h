#ifndef MAP_H
#define MAP_H

#include "datastructures.h"
#include "ingameelements/movingentity.h"
#include "ingameelements/character.h"

#include <allegro5/allegro.h>
#include <string>

class Map
{
    public:
        Map(std::string name);
        ~Map();
        void renderbackground(double cam_x, double cam_y);
        void renderwallground(double cam_x, double cam_y);
        bool collides(Gamestate *state, MovingEntity *entity);
        bool collides(Gamestate *state, Character *entity);
        bool collides(Rect rect);
        bool testpixel(double x, double y);
    protected:
    private:
        ALLEGRO_BITMAP *background;
        ALLEGRO_BITMAP *wallground;
        ALLEGRO_BITMAP *wallmask;
};

#endif // MAP_H
