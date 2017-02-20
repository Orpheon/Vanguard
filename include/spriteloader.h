#pragma once

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
        ALLEGRO_BITMAP* requestsprite(std::string path, double zoom_);
        ALLEGRO_BITMAP* requestspriteoutline(std::string path);
        ALLEGRO_BITMAP* requestspriteoutline(std::string path, double zoom_);

        Spriteloader & operator=(Spriteloader &&)=default;
        int get_spriteoffset_x(std::string s);
        int get_spriteoffset_y(std::string s);
        Rect get_rect(std::string s);
        Rect get_rect_from_json(std::string s);
        int getweaponoffset_x(std::string s);
        int getweaponoffset_y(std::string s);
        void clearcache();
        void setzoom(double zoom);
    protected:
    private:
        std::unordered_map<std::string, ALLEGRO_BITMAP*> bitmapcache;
        nlohmann::json spriteoffsets;
        nlohmann::json gamedata;
        bool masksonly;
        double defaultzoom;
};

