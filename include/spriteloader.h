#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <unordered_map>
#include <allegro5/allegro.h>
#include <string>

class Spriteloader
{
    public:
        Spriteloader(bool memory_only);
        virtual ~Spriteloader();

        ALLEGRO_BITMAP* request_sprite(std::string path);

        Spriteloader & operator=(Spriteloader &&)=default;
    protected:
    private:
        std::unordered_map<std::string, ALLEGRO_BITMAP*> bitmapcache;
};

#endif // SPRITELOADER_H
