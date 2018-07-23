#pragma once

#include <unordered_map>
#include <string>

#include <SFML/Graphics/Image.hpp>
#include "json.hpp"

#include "datastructures.h"

class Maskloader
{
    public:
        Maskloader();
        virtual ~Maskloader();
        sf::Image& loadmask(std::string &path);
        Rect get_rect(std::string s);
        sf::Vector2i weaponoffset(std::string s);
        void clearcache();
        Maskloader & operator=(Maskloader &&)=default;

    protected:
    private:
        std::unordered_map<std::string, sf::Image> maskcache;
        nlohmann::json gamedata;
};