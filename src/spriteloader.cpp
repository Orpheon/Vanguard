#include "spriteloader.h"

Spriteloader::Spriteloader() : bitmapcache()
{
    //ctor
}

Spriteloader::~Spriteloader()
{
    for (auto e : bitmapcache)
    {
        al_destroy_bitmap(e);
    }
}

ALLEGRO_BITMAP* request_sprite(std::string path)
{
    if (bitmapcache.count(path) == 0)
    {
        bitmapcache[path] = al_load_bitmap(path);
    }
    return bitmapcache[path];
}
