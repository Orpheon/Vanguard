#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <unordered_map>
#include <allegro5/allegro.h>
#include <string>
#include "json.hpp"
#include "datastructures.h"

class Spriteloader
{
    public:
        Spriteloader(bool memory_only);
        virtual ~Spriteloader();

        ALLEGRO_BITMAP* requestsprite(std::string path);

        Spriteloader & operator=(Spriteloader &&)=default;
        int get_spriteoffset_x(std::string s);
        int get_spriteoffset_y(std::string s);
        Rect get_rect(std::string s);
        Rect get_rect_from_json(std::string s);
        int getweaponoffset_x(std::string s);
        int getweaponoffset_y(std::string s);
    protected:
    private:
        std::unordered_map<std::string, ALLEGRO_BITMAP*> bitmapcache;
        nlohmann::json spriteoffsets;
        nlohmann::json gamedata;
        bool MEMORY_ONLY;
};

#endif // SPRITELOADER_H
