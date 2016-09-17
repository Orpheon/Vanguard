#include "spriteloader.h"

Spriteloader::Spriteloader() : bitmapcache()
{
    //ctor
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
        al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
        bitmapcache[path] = al_load_bitmap(path.c_str());
        if (bitmapcache[path] == NULL)
        {
            printf("\nERROR: Could not load %s!", path.c_str());
            return 0;
        }
    }
    return bitmapcache[path];
}
