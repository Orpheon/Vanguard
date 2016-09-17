#include "maskloader.h"

Maskloader::Maskloader()
{
    //ctor
}

Maskloader::~Maskloader()
{
    for (auto e : bitmapcache)
    {
        al_destroy_bitmap(e.second);
    }
}

ALLEGRO_BITMAP* Maskloader::request_mask(std::string path)
{
    if (bitmapcache.count(path) == 0)
    {
        al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
        bitmapcache[path] = al_load_bitmap(path.c_str());
        if (bitmapcache[path] == NULL)
        {
            printf("\nERROR: Could not load %s!", path.c_str());
            return 0;
        }
    }
    return bitmapcache[path];
}
