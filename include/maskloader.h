#ifndef MASKLOADER_H
#define MASKLOADER_H

#include <unordered_map>
#include <allegro5/allegro.h>
#include <string>

class Maskloader
{
    public:
        Maskloader();
        virtual ~Maskloader();

        ALLEGRO_BITMAP* request_mask(std::string path);
        Maskloader & operator=(Maskloader &&)=default;
    protected:
    private:
        std::unordered_map<std::string, ALLEGRO_BITMAP*> bitmapcache;
};

#endif // MASKLOADER_H
