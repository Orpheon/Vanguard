#include "spriteloader.h"
#include <fstream>

Spriteloader::Spriteloader(bool masksonly_) : bitmapcache(), masksonly(masksonly_)
{
    // Load the sprite offset data
    std::ifstream spriteoffsetsfile("sprites/spritedata.json");
    spriteoffsets << spriteoffsetsfile;
    spriteoffsetsfile.close();

    // Load the game data
    std::ifstream gamedatafile("gamedata.json");
    gamedata << gamedatafile;
    gamedatafile.close();
}

Spriteloader::~Spriteloader()
{
    for (auto e : bitmapcache)
    {
        al_destroy_bitmap(e.second);
    }
}

int Spriteloader::get_spriteoffset_x(std::string s)
{
    if (masksonly)
    {
        if (spriteoffsets.find(s+"_hitmask.png") != spriteoffsets.end())
        {
            return spriteoffsets[s+"_hitmask.png"][0];
        }
    }

    s += "_sprite.png";
    try
    {
        return spriteoffsets[s][0];
    }
    catch (std::domain_error)
    {
        fprintf(stderr, "\nError: Could not load sprite offset of %s!", s.c_str());
        throw -1;
    }
}

int Spriteloader::get_spriteoffset_y(std::string s)
{
    if (masksonly)
    {
        if (spriteoffsets.find(s+"_hitmask.png") != spriteoffsets.end())
        {
            return spriteoffsets[s+"_hitmask.png"][1];
        }
    }

    s += "_sprite.png";
    try
    {
        return spriteoffsets[s][1];
    }
    catch (std::domain_error)
    {
        fprintf(stderr, "\nError: Could not load sprite offset of %s!", s.c_str());
        throw -1;
    }
}

ALLEGRO_BITMAP* Spriteloader::requestsprite(std::string path)
{
    if (bitmapcache.count(path) == 0)
    {
        if (masksonly)
        {
            al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
            bitmapcache[path] = al_load_bitmap(("sprites/"+path+"_hitmask.png").c_str());
        }
        if (not masksonly or bitmapcache[path] == NULL)
        {
            al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
            bitmapcache[path] = al_load_bitmap(("sprites/"+path+"_sprite.png").c_str());
        }
        if (bitmapcache[path] == NULL)
        {
            fprintf(stderr, "\nError: Could not load sprites/%s_sprite.png!", path.c_str());
            return 0;
        }
    }
    return bitmapcache[path];
}

ALLEGRO_BITMAP* Spriteloader::requestspriteoutline(std::string path)
{
    path += "_outline";
    if (bitmapcache.count(path) == 0)
    {
        if (masksonly)
        {
            al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
        }
        else
        {
            al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
        }
        bitmapcache[path] = al_load_bitmap(("sprites/"+path+"_outline.png").c_str());
        if (bitmapcache[path] == NULL)
        {
            fprintf(stderr, "\nError: Could not load sprites/%s.png!", path.c_str());
            return 0;
        }
    }
    return bitmapcache[path];
}

Rect Spriteloader::get_rect(std::string s)
{
    ALLEGRO_BITMAP *sprite = requestsprite(s);
    int dx = get_spriteoffset_x(s), dy = get_spriteoffset_y(s);
    return Rect(-dx, -dy, al_get_bitmap_width(sprite), al_get_bitmap_height(sprite));
}

Rect Spriteloader::get_rect_from_json(std::string s)
{
    try
    {
        return Rect(gamedata[s+" rect"][0], gamedata[s+" rect"][1], gamedata[s+" rect"][2], gamedata[s+" rect"][3]);
    }
    catch (std::domain_error)
    {
        fprintf(stderr, "\nError: Could not load %s rect data!", s.c_str());
        throw -1;
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
