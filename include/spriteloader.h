#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <unordered_map>
#include <allegro5/allegro.h>
#include <string>
#include "json.hpp"

class Spriteloader
{
    public:
        Spriteloader(bool memory_only);
        virtual ~Spriteloader();

        ALLEGRO_BITMAP* request_sprite(std::string path);

        Spriteloader & operator=(Spriteloader &&)=default;
        int get_spriteoffset_x(std::string s) {return spriteoffsets[s][0];}
        int get_spriteoffset_y(std::string s) {return spriteoffsets[s][1];}
    protected:
    private:
        std::unordered_map<std::string, ALLEGRO_BITMAP*> bitmapcache;
        nlohmann::json spriteoffsets;
        bool MEMORY_ONLY;
};

#endif // SPRITELOADER_H
