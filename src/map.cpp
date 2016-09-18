#include <allegro5/allegro.h>
#include <string>
#include <fstream>

#include "map.h"
#include "gamestate.h"
#include "engine.h"

Map::Map(std::string name)
{
    // Load and scale the map (background and then wallmask)
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
    background = al_load_bitmap(("maps/"+name+".png").c_str());

    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    wallmask = al_load_bitmap(("maps/"+name+"_wm.png").c_str());
    al_lock_bitmap(wallmask, al_get_bitmap_format(wallmask), ALLEGRO_LOCK_READONLY);
}

Map::~Map()
{
    al_destroy_bitmap(background);
    al_unlock_bitmap(wallmask);
    al_destroy_bitmap(wallmask);
}

void Map::render(double cam_x, double cam_y)
{
    al_draw_bitmap(background, -cam_x, -cam_y, 0);
}

bool Map::collides(Gamestate *state, MovingEntity *entity)
{
    ALLEGRO_BITMAP *mask = state->engine->maskloader.request_mask(entity->mask);
    int w = al_get_bitmap_width(mask), h = al_get_bitmap_height(mask);
    if (entity->x < 0 or entity->y < 0 or entity->x+w > al_get_bitmap_width(wallmask) or entity->y+h > al_get_bitmap_height(wallmask))
    {
        return true;
    }
    int x = (int) entity->x, y = (int) entity->y;
    for (int i=0; i<w; ++i)
    {
        for (int j=0; j<h; ++j)
        {
            if (al_get_pixel(wallmask, i+x, j+y).a != 0 and al_get_pixel(mask, i, j).a != 0)
            {
                return true;
            }
        }
    }
    return false;
}
