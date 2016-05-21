#include <allegro5/allegro.h>

#include "map.h"

Map::Map(char *name)
{
    background = al_load_bitmap(name);
}

Map::~Map()
{
	al_destroy_bitmap(background);
}

void Map::render()
{
	al_draw_bitmap(background, 0, 0, 0);
}
