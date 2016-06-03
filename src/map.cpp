#include <allegro5/allegro.h>
#include <string>

#include "map.h"

Map::Map(std::string name)
{
    // Load and scale the background
    ALLEGRO_BITMAP *tmpbmp = al_load_bitmap(("maps/"+name+".png").c_str());
    int w = al_get_bitmap_width(tmpbmp), h = al_get_bitmap_height(tmpbmp);
    background = al_create_bitmap(w*6, h*6);
    al_set_target_bitmap(background);
    al_draw_scaled_bitmap(tmpbmp, 0, 0, w, h, 0, 0, 6*w, 6*h, 0);
    al_destroy_bitmap(tmpbmp);

    // Load and scale the wallmask
    tmpbmp = al_load_bitmap(("maps/"+name+"_wm.png").c_str());
    w = al_get_bitmap_width(tmpbmp);
    h = al_get_bitmap_height(tmpbmp);
    wallmask = al_create_bitmap(w*6, h*6);
    al_set_target_bitmap(wallmask);
    al_draw_scaled_bitmap(tmpbmp, 0, 0, w, h, 0, 0, 6*w, 6*h, 0);
    al_destroy_bitmap(tmpbmp);
}

Map::~Map()
{
    al_destroy_bitmap(background);
}

void Map::render()
{
    al_draw_bitmap(background, 0, 0, 0);
}
