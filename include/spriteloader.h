#pragma once

#include <unordered_map>
#include <string>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "json.hpp"

#include "datastructures.h"

class Spriteloader
{
    public:
        Spriteloader();
        virtual ~Spriteloader();
        void loadsprite(std::string path, sf::Sprite& sprite);
        void loadspriteoutline(std::string path, sf::Sprite& sprite);
        Rect get_rect(std::string path);
        void clearcache();
        Spriteloader & operator=(Spriteloader &&)=default;

    protected:
    private:
        std::unordered_map<std::string, sf::Texture> texturecache;
        nlohmann::json spriteoffsets;
};

