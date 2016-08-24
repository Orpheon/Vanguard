#include <allegro5/allegro.h>
#include <zip.h>
#include <string>
#include <fstream>

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

    // Load and convert the wallmask to bitmap
    std::ifstream wm_file("maps/"+name+"_wm.wallmask");

    wallmask.width = w*6;
    wallmask.height = h*6;
    wallmask.bitmap = (bool*) calloc(wallmask.width*wallmask.height, 1);
    bool pixel;
    for (int x=0; x<w; ++x)
    {
        for (int y=0; y<h; ++y)
        {
            // Wew the hacks. This will interpret anything that's not a 1 as a a 0, but that shouldn't be an issue.
            pixel = (wm_file.get() == '1');
            for (int i=0; i<6; ++i)
            {
                for (int j=0; j<6; ++j)
                {
                    wallmask.bitmap[(x+i)*wallmask.height + y+j] = pixel;
                }
            }
        }
    }
}

Map::~Map()
{
    al_destroy_bitmap(background);
    free(wallmask.bitmap);
}

void Map::render(double cam_x, double cam_y)
{
    al_draw_bitmap(background, -cam_x, -cam_y, 0);
}
