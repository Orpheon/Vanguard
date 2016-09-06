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
        bitmapcache[path] = al_load_bitmap(path.c_str());
    }
    return bitmapcache[path];
}
