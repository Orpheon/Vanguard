#include "maskloader.h"
#include "configloader.h"
#include "global.h"

#include <fstream>

Maskloader::Maskloader() : maskcache()
{
    ConfigLoader cfgloader;

    gamedata = cfgloader.open("gamedata.json");
    spriteoffsets = cfgloader.open("sprites/spritedata.json");
}

Maskloader::~Maskloader()
{

}

sf::Image& Maskloader::loadmask(std::string path)
{
    if (maskcache.count(path) == 0)
    {
        sf::Image newimage;
        if (not newimage.loadFromFile("sprites/" + path + "_hitmask.png"))
        {
            if (not newimage.loadFromFile("sprites/" + path + "_sprite.png"))
            {
                Global::logging().panic(__FILE__, __LINE__, " Could not load sprites/%s_sprite.png", path.c_str());
            }
        }
        maskcache[path] = newimage;
    }

    return maskcache[path];
}

sf::Vector2i Maskloader::offsets(std::string path)
{
    std::string fullpath = path + "_sprite.png";
    int x, y;
    try
    {
        x = spriteoffsets[fullpath][0];
        y = spriteoffsets[fullpath][1];
    }
    catch (std::domain_error)
    {
        Global::logging().panic(__FILE__, __LINE__, "Could not load sprite offset of %s", fullpath.c_str());
    }
    return sf::Vector2i(x, y);
}

Rect Maskloader::get_rect(std::string s)
{
    sf::Image &mask = loadmask(s);
    sf::Vector2u size = mask.getSize();
    return Rect(0, 0, size.x, size.y);
}

Rect Maskloader::get_json_rect(std::string s)
{
    try
    {
        return Rect(gamedata[s+" rect"][0], gamedata[s+" rect"][1], gamedata[s+" rect"][2], gamedata[s+" rect"][3]);
    }
    catch (std::domain_error)
    {
        Global::logging().panic(__FILE__, __LINE__, " Could not load %s rect data", s.c_str());
        return Rect();
    }
}

sf::Vector2i Maskloader::weaponoffsets(std::string s)
{
    try
    {
        return sf::Vector2i(gamedata[s+" weapon"][0], gamedata[s+" weapon"][1]);
    }
    catch (std::domain_error)
    {
        return sf::Vector2i(0, 0);
    }
}

void Maskloader::clearcache()
{
    maskcache.clear();
}