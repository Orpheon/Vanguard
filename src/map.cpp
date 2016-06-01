#include <allegro5/allegro.h>

#include "map.h"

Map::Map(char *name)
{
    // Load and scale the background
    ALLEGRO_BITMAP *tmpbg = al_load_bitmap(name);
    int w = al_get_bitmap_width(tmpbg), h = al_get_bitmap_height(tmpbg);
    background = al_create_bitmap(w*6, h*6);
    al_set_target_bitmap(background);
    al_draw_scaled_bitmap(tmpbg, 0, 0, w, h, 0, 0, 6*w, 6*h, 0);
    al_destroy_bitmap(tmpbg);
}

Map::~Map()
{
    al_destroy_bitmap(background);
}

void Map::render()
{
    al_draw_bitmap(background, 0, 0, 0);
}
