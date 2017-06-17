#include "spriteloader.h"
#include "configloader.h"
#include "global.h"

#include <fstream>

Spriteloader::Spriteloader(bool masksonly_) : bitmapcache(), masksonly(masksonly_), defaultzoom(1.0)
{
    ConfigLoader cfgloader;

    spriteoffsets = cfgloader.open("sprites/spritedata.json");
    gamedata = cfgloader.open("gamedata.json");
}

Spriteloader::~Spriteloader()
{
    for (auto &e : bitmapcache)
    {
        al_destroy_bitmap(e.second);
    }
}

int Spriteloader::get_spriteoffset_x(std::string s)
{
    int offset;
    if (masksonly)
    {
        if (spriteoffsets.find(s+"_hitmask.png") != spriteoffsets.end())
        {
            offset = spriteoffsets[s+"_hitmask.png"][0];
            return offset;
        }
    }

    s += "_sprite.png";
    try
    {
        offset = spriteoffsets[s][0];
    }
    catch (std::domain_error)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not load sprite offset of %s", s.c_str());
    }
    return offset;
}

int Spriteloader::get_spriteoffset_y(std::string s)
{
    int offset;
    if (masksonly)
    {
        if (spriteoffsets.find(s+"_hitmask.png") != spriteoffsets.end())
        {
            offset = spriteoffsets[s+"_hitmask.png"][1];
            return offset;
        }
    }

    s += "_sprite.png";
    try
    {
        offset = spriteoffsets[s][1];
    }
    catch (std::domain_error)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not load sprite offset of %s", s.c_str());
    }
    return offset;
}

ALLEGRO_BITMAP* Spriteloader::requestsprite(std::string path)
{
    if (bitmapcache.count(path) == 0)
    {
        ALLEGRO_BITMAP *tmpbitmap = NULL;
        if (masksonly)
        {
            al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
            tmpbitmap = al_load_bitmap(("sprites/"+path+"_hitmask.png").c_str());
            if (tmpbitmap == NULL)
            {
                al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
                tmpbitmap = al_load_bitmap(("sprites/"+path+"_sprite.png").c_str());
            }
        }
        else
        {
            al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
            tmpbitmap = al_load_bitmap(("sprites/"+path+"_sprite.png").c_str());
        }
        if (tmpbitmap == NULL)
        {
            Global::logging().panic(__FILE__, __LINE__, " Could not load sprites/%s_sprite.png", path.c_str());
        }
        if (defaultzoom == 1.0)
        {
            bitmapcache[path] = tmpbitmap;
        }
        else
        {
            bitmapcache[path] = tmpbitmap;
            int w=al_get_bitmap_width(tmpbitmap), h=al_get_bitmap_height(tmpbitmap);
            bitmapcache[path] = al_create_bitmap(w*defaultzoom, h*defaultzoom);
            ALLEGRO_BITMAP *oldtarget = al_get_target_bitmap();
            al_set_target_bitmap(bitmapcache[path]);
            al_clear_to_color(al_map_rgba(0, 0, 0, 0));
            al_draw_scaled_bitmap(tmpbitmap, 0, 0, w, h, 0, 0, w*defaultzoom, h*defaultzoom, 0);
            al_set_target_bitmap(oldtarget);
        }
    }
    return bitmapcache.at(path);
}

ALLEGRO_BITMAP* Spriteloader::requestspriteoutline(std::string path)
{
    path += "_outline";
    if (bitmapcache.count(path) == 0)
    {
        ALLEGRO_BITMAP *tmpbitmap = NULL;
        if (masksonly)
        {
            al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
        }
        else
        {
            al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
        }
        tmpbitmap = al_load_bitmap(("sprites/"+path+".png").c_str());
        if (tmpbitmap == NULL)
        {
            Global::logging().panic(__FILE__, __LINE__, " Could not load sprites/%s.png", path.c_str());

        }
        if (defaultzoom == 1.0)
        {
            bitmapcache[path] = tmpbitmap;
        }
        else
        {
            bitmapcache[path] = tmpbitmap;
            int w=al_get_bitmap_width(tmpbitmap), h=al_get_bitmap_height(tmpbitmap);
            bitmapcache[path] = al_create_bitmap(w*defaultzoom, h*defaultzoom);
            ALLEGRO_BITMAP *oldtarget = al_get_target_bitmap();
            al_set_target_bitmap(bitmapcache.at(path));
            al_draw_scaled_bitmap(tmpbitmap, 0, 0, w, h, 0, 0, w*defaultzoom, h*defaultzoom, 0);
            al_set_target_bitmap(oldtarget);
        }
    }
    return bitmapcache.at(path);
}

Rect Spriteloader::get_rect(std::string s)
{
    ALLEGRO_BITMAP *sprite = requestsprite(s);
    int dx = get_spriteoffset_x(s), dy = get_spriteoffset_y(s);
    return Rect(-dx * defaultzoom, -dy * defaultzoom, al_get_bitmap_width(sprite), al_get_bitmap_height(sprite));
}

Rect Spriteloader::get_rect_from_json(std::string s)
{
    try
    {
        return Rect(gamedata[s+" rect"][0], gamedata[s+" rect"][1], gamedata[s+" rect"][2], gamedata[s+" rect"][3]);
    }
    catch (std::domain_error)
    {
        Global::logging().panic(__FILE__, __LINE__, " Could not load %s rect data", s.c_str());
        return Rect();
    }
}

int Spriteloader::getweaponoffset_x(std::string s)
{
    try
    {
        return gamedata[s+" weapon"][0];
    }
    catch (std::domain_error)
    {
        return 0;
    }
}

int Spriteloader::getweaponoffset_y(std::string s)
{
    try
    {
        return gamedata[s+" weapon"][1];
    }
    catch (std::domain_error)
    {
        return 0;
    }
}

void Spriteloader::clearcache()
{
    for (auto &e : bitmapcache)
    {
        al_destroy_bitmap(e.second);
    }
    bitmapcache.clear();
}

void Spriteloader::setzoom(double zoom)
{
    defaultzoom = zoom;
    clearcache();
}
