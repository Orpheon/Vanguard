#include "spriteloader.h"
#include "configloader.h"
#include "global.h"

#include <fstream>

Spriteloader::Spriteloader() : texturecache()
{
    ConfigLoader cfgloader;

    spriteoffsets = cfgloader.open("sprites/spritedata.json");
}

Spriteloader::~Spriteloader()
{

}

void Spriteloader::loadsprite(std::string path, sf::Sprite &sprite)
{
    if (texturecache.count(path) == 0)
    {
        sf::Texture newtexture;
        if (not newtexture.loadFromFile("sprites/" + path + "_sprite.png"))
        {
            Global::logging().panic(__FILE__, __LINE__, " Could not load sprites/%s_sprite.png", path.c_str());
        }
        texturecache[path] = newtexture;
    }

    sprite.setTexture(texturecache.at(path));

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

    sprite.setOrigin(x, y);
}

void Spriteloader::loadspriteoutline(std::string path, sf::Sprite &sprite)
{
    std::string outlinepath = path + "_outline";
    if (texturecache.count(outlinepath) == 0)
    {
        sf::Texture newtexture;
        if (not newtexture.loadFromFile("sprites/" + outlinepath))
        {
            Global::logging().panic(__FILE__, __LINE__, " Could not load sprites/%s_sprite.png", outlinepath.c_str());
        }
        texturecache[outlinepath] = newtexture;
    }

    sprite.setTexture(texturecache.at(outlinepath));

    std::string fullpath = outlinepath + "_sprite.png";
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

    sprite.setOrigin(x, y);
}

Rect Spriteloader::get_rect(std::string path)
{
    sf::Sprite sprite;
    loadsprite(path, sprite);
    sf::FloatRect rect = sprite.getLocalBounds();
    Rect r(rect.left, rect.top, rect.width, rect.height);
    return r;
}

void Spriteloader::clearcache()
{
    texturecache.clear();
}