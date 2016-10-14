#include "spriteloader.h"
#include <fstream>

Spriteloader::Spriteloader(bool memory_only) : bitmapcache()
{
    // Load the sprite offset data
    std::ifstream spriteoffsetsfile("spriteoffsets.json");
    spriteoffsets << spriteoffsetsfile;
    spriteoffsetsfile.close();

    MEMORY_ONLY = memory_only;
}

Spriteloader::~Spriteloader()
{
    for (auto e : bitmapcache)
    {
        al_destroy_bitmap(e.second);
    }
}

ALLEGRO_BITMAP* Spriteloader::request_sprite(std::string path)
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
        bitmapcache[path] = al_load_bitmap(("sprites/"+path).c_str());
        if (bitmapcache[path] == NULL)
        {
            printf("\nERROR: Could not load %s!", path.c_str());
            printf("\n");
            return 0;
        }
    }
    return bitmapcache[path];
}

Rect Spriteloader::get_rect(std::string s)
{
    ALLEGRO_BITMAP *sprite = request_sprite(s);
    int dx = get_spriteoffset_x(s), dy = get_spriteoffset_y(s);
    return Rect(-dx, -dy, al_get_bitmap_width(sprite), al_get_bitmap_height(sprite));
}
