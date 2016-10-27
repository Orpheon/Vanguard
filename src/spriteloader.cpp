#include "spriteloader.h"
#include <fstream>

Spriteloader::Spriteloader(bool memory_only) : bitmapcache()
{
    // Load the sprite offset data
    std::ifstream spriteoffsetsfile("sprites/spritedata.json");
    spriteoffsets << spriteoffsetsfile;
    spriteoffsetsfile.close();

    // Load the game data
    std::ifstream gamedatafile("gamedata.json");
    gamedata << gamedatafile;
    gamedatafile.close();

    MEMORY_ONLY = memory_only;
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
    s += ".png";
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
    s += ".png";
    try
    {
        return spriteoffsets[s][1];
    }
    catch (std::domain_error)
    {
        fprintf(stderr, "\nError: Could not load sprite offset %s!\n", s.c_str());
        throw -1;
    }
}

ALLEGRO_BITMAP* Spriteloader::requestsprite(std::string path)
{
    if (bitmapcache.count(path) == 0)
    {
        if (MEMORY_ONLY)
        {
            al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
        }
        else
        {
            al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
        }
//        ALLEGRO_BITMAP *tmp = al_load_bitmap(("sprites/"+path).c_str());
//        int w = al_get_bitmap_width(tmp), h = al_get_bitmap_height(tmp);
//        bitmapcache[path] = al_create_bitmap(w*2, h*2);
//        al_set_target_bitmap(bitmapcache[path]);
//        al_draw_scaled_rotated_bitmap(tmp, 0, 0, 0, 0, 2, 2, 0, 0);
//        al_destroy_bitmap(tmp);
        bitmapcache[path] = al_load_bitmap(("sprites/"+path+".png").c_str());
        if (bitmapcache[path] == NULL)
        {
            fprintf(stderr, "\nError: Could not load %s!", path.c_str());
            return 0;
        }
    }
    return bitmapcache[path];
}

Rect Spriteloader::get_rect(std::string s)
{
    ALLEGRO_BITMAP *sprite = requestsprite(s);
//    int dx = get_spriteoffset_x(s), dy = get_spriteoffset_y(s);
//    return Rect(-dx, -dy, al_get_bitmap_width(sprite), al_get_bitmap_height(sprite));
    return Rect(0, 0, al_get_bitmap_width(sprite), al_get_bitmap_height(sprite));
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
