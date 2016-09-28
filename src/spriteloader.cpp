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
