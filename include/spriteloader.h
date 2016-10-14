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

        ALLEGRO_BITMAP* request_sprite(std::string path);

        Spriteloader & operator=(Spriteloader &&)=default;
        int get_spriteoffset_x(std::string s) {return spriteoffsets[s][0];}
        int get_spriteoffset_y(std::string s) {return spriteoffsets[s][1];}
        Rect get_rect(std::string s);
        Rect get_rect_from_json(std::string s) {return Rect(spriteoffsets[s+" rect"][0], spriteoffsets[s+" rect"][1], spriteoffsets[s+" rect"][2], spriteoffsets[s+" rect"][3]);}
    protected:
    private:
        std::unordered_map<std::string, ALLEGRO_BITMAP*> bitmapcache;
        nlohmann::json spriteoffsets;
        bool MEMORY_ONLY;
};

#endif // SPRITELOADER_H
