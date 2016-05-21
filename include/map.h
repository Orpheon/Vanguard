#ifndef MAP_H
#define MAP_H

#include <allegro5/allegro.h>

class Map
{
    public:
        Map(char *name);
        ~Map();
        void render();
    protected:
    private:
        ALLEGRO_BITMAP *background;
};

#endif // MAP_H
