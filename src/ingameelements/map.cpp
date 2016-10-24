#include <allegro5/allegro.h>
#include <string>
#include <fstream>

#include "ingameelements/map.h"
#include "gamestate.h"
#include "engine.h"

Map::Map(std::string name)
{
    // Load the map
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
    background = al_load_bitmap(("maps/"+name+".png").c_str());
    wallground = al_load_bitmap(("maps/"+name+"_wm.png").c_str());

    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    wallmask = al_load_bitmap(("maps/"+name+"_wm.png").c_str());
    al_lock_bitmap(wallmask, al_get_bitmap_format(wallmask), ALLEGRO_LOCK_READONLY);
}

Map::~Map()
{
    al_destroy_bitmap(background);
    al_destroy_bitmap(wallground);
    al_unlock_bitmap(wallmask);
    al_destroy_bitmap(wallmask);
}

void Map::renderbackground(double cam_x, double cam_y)
{
    al_draw_bitmap(background, -cam_x, -cam_y, 0);
}

void Map::renderwallground(double cam_x, double cam_y)
{
    al_draw_bitmap(wallground, -cam_x, -cam_y, 0);
}

bool Map::collides(Gamestate *state, MovingEntity *entity)
{
    ALLEGRO_BITMAP *mask = state->engine->maskloader.requestsprite(entity->getsprite(state, true));
    int x = (int) entity->x - state->engine->maskloader.get_spriteoffset_x(entity->getsprite(state, true));
    int y = (int) entity->y - state->engine->maskloader.get_spriteoffset_y(entity->getsprite(state, true));

    int w = al_get_bitmap_width(mask), h = al_get_bitmap_height(mask);
    if (x < 0 or y < 0 or x+w > al_get_bitmap_width(wallmask) or y+h > al_get_bitmap_height(wallmask))
    {
        return true;
    }
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

bool Map::collides(Gamestate *state, Character *entity)
{
    ALLEGRO_BITMAP *mask = state->engine->maskloader.requestsprite(entity->getsprite(state, true));
    int x = (int) entity->x - state->engine->maskloader.get_spriteoffset_x(entity->getsprite(state, true));
    int y = (int) entity->y - state->engine->maskloader.get_spriteoffset_y(entity->getsprite(state, true));
    if (entity->isflipped)
    {
        x = (int) entity->x - (al_get_bitmap_width(mask) - state->engine->maskloader.get_spriteoffset_x(entity->getsprite(state, true)));
    }

    int w = al_get_bitmap_width(mask), h = al_get_bitmap_height(mask);
    if (x < 0 or y < 0 or x+w > al_get_bitmap_width(wallmask) or y+h > al_get_bitmap_height(wallmask))
    {
        return true;
    }
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

bool Map::collides(Rect r)
{
    if (r.x < 0 or r.y < 0 or r.x+r.w > al_get_bitmap_width(wallmask) or r.y+r.h > al_get_bitmap_height(wallmask))
    {
        return true;
    }
    for (int i=0; i<r.w; ++i)
    {
        for (int j=0; j<r.h; ++j)
        {
            if (al_get_pixel(wallmask, i+r.x, j+r.y).a != 0)
            {
                return true;
            }
        }
    }
    return false;
}
