#include <string>
#include <fstream>

#include "ingameelements/map.h"
#include "gamestate.h"
#include "engine.h"
#include "mapelements/spawnroom.h"
#include "mapelements/controlpoint.h"
#include "renderer.h"
#include "configloader.h"
#include "ingameelements/gamemodes/gamemodemanager.h"
#include "ingameelements/gamemodes/kothmanager.h"

Map::Map(Gamestate &state, std::string name_)
{
    name = name_;

    // Load the map data first
    std::string mapfolder = "maps/" + name + "/";
    ConfigLoader configloader;
    mapdata = configloader.open(mapfolder + "mapdata.json");

    // Load all the images
    std::string bg = mapdata.at("background"), wg = mapdata.at("wallmask foreground"), wm = mapdata.at("wallmask");
    background.loadFromFile(mapfolder + bg);
    wallground.loadFromFile(mapfolder + wg);
    wallmask.loadFromFile(mapfolder + wm);

    if (mapdata.find("gamemodes") == mapdata.end())
    {
        Global::logging().panic(__FILE__, __LINE__, "%s does not have proper gamemode data", (mapfolder+"mapdata.json").c_str());
    }

    for (nlohmann::json gamemode : mapdata.at("gamemodes"))
    {
        if (gamemode.at("type") == "KOTH")
        {
            if (gamemode.find("cp") == gamemode.end())
            {
                Global::logging().panic(__FILE__, __LINE__, "%s koth gamemode does not have a cp field.", (mapfolder+"mapdata.json").c_str());
            }
            if (gamemode.find("spawn 1") == gamemode.end())
            {
                Global::logging().panic(__FILE__, __LINE__, "%s koth gamemode does not have a spawn 1 field.", (mapfolder+"mapdata.json").c_str());
            }
            if (gamemode.find("spawn 2") == gamemode.end())
            {
                Global::logging().panic(__FILE__, __LINE__, "%s koth gamemode does not have a spawn 2 field.", (mapfolder+"mapdata.json").c_str());
            }
            Rect cparea(gamemode.at("cp")[0], gamemode.at("cp")[1], gamemode.at("cp")[2], gamemode.at("cp")[3]);
            std::vector<Rect> spawnarea1, spawnarea2;
            for (auto &r : gamemode.at("spawn 1"))
            {
                Rect newrect(r[0], r[1], r[2], r[3]);
                spawnarea1.push_back(newrect);
            }
            for (auto &r : gamemode.at("spawn 2"))
            {
                Rect newrect(r[0], r[1], r[2], r[3]);
                spawnarea2.push_back(newrect);
            }
            gamemodes.push_back(state.make_entity<KothManager>(spawnarea1, spawnarea2, cparea));
        }
        else
        {
            Global::logging().panic(__FILE__, __LINE__, "Unknown gamemode %s", gamemode.at("type"));
        }
    }
    currentgamemode(state).activate(state, std::bind(&Map::gotonextgamemode, this, std::placeholders::_1, std::placeholders::_2));
}

Map::~Map()
{

}

void Map::renderbackground(Renderer &renderer)
{
    sf::Sprite sprite(background);
    renderer.background.draw(sprite);
}

void Map::renderwallground(Renderer &renderer)
{
    sf::Sprite sprite(wallground);
    renderer.surfaceground.draw(sprite);
}

bool Map::testpixel(double x, double y)
{
    if (x >= 0 and y >= 0)
    {
        sf::Vector2u mapsize = wallmask.getSize();
        if (x < mapsize.x and y < mapsize.y)
        {
            return wallmask.getPixel(x, y).a != 0;
        }
    }
    return true;
}

bool Map::collides(Rect r)
{
    sf::Vector2u mapsize = wallmask.getSize();
    if (r.x < 0 or r.y < 0 or r.x+r.w > mapsize.x or r.y+r.h > mapsize.y)
    {
        return true;
    }
    for (int i=0; i<r.w; ++i)
    {
        for (int j=0; j<r.h; ++j)
        {
            if (wallmask.getPixel(i+r.x, j+r.y).a != 0)
            {
                return true;
            }
        }
    }
    return false;
}

bool Map::collides(Gamestate &state, double x, double y, std::string spriteid, double angle)
{
    sf::Vector2u mapsize = wallmask.getSize();
    Rect bbox = state.engine.maskloader.get_rect(spriteid);

    // Check if projectile is outside the map somehow
    if (x + bbox.x > mapsize.x or y + bbox.y > mapsize.y or x + bbox.x + bbox.w < 0 or y + bbox.y + bbox.h < 0)
    {
        return true;
    }
    // Check pixel-wise
    sf::Image &image = state.engine.maskloader.loadmask(spriteid);
    sf::Vector2i offsets = state.engine.maskloader.offsets(spriteid);
    double cosa = std::cos(angle), sina = std::sin(angle);
    for (int i = 0; i < bbox.w; ++i)
    {
        double relx = i - offsets.x;
        for (int j = 0; j < bbox.h; ++j)
        {
            if (wallmask.getPixel(i, j).a != 0)
            {
                // Rotate around (x,y) by angle
                double rely = j - offsets.y;
                double rotx = x + cosa*relx - sina*rely;
                double roty = y + sina*relx + cosa*rely;
                if (image.getPixel(rotx, roty).a != 0)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Map::collideline(double x1, double y1, double x2, double y2)
{
    sf::Vector2u mapsize = wallmask.getSize();
    if (x1 < 0 or y1 < 0 or x2 < 0 or y2 < 0 or x1 > mapsize.x or x2 > mapsize.x or y1 > mapsize.y or y2 > mapsize.y)
    {
        return true;
    }

    int nsteps = std::ceil(std::max(std::abs(x1-x2), std::abs(y1-y2)));
    double dx = static_cast<double>(x2-x1)/nsteps, dy = static_cast<double>(y2-y1)*1.0/nsteps;
    for (int i=0; i<nsteps; ++i)
    {
        if (wallmask.getPixel(x1, y1).a != 0)
        {
            return true;
        }
        x1 += dx; y1 += dy;
    }
    return false;
}

Spawnroom& Map::spawnroom(Gamestate &state, Team team)
{
    return state.get<Spawnroom>(currentgamemode(state).spawnrooms.at(team));
}

GamemodeManager& Map::currentgamemode(Gamestate &state)
{
    return state.get<GamemodeManager>(gamemodes.front());
}

void Map::gotonextgamemode(Gamestate &state, Team winners)
{
    currentgamemode(state).destroy(state);
    gamemodes.pop_front();
    if (gamemodes.size() == 0)
    {
        state.mapend();
    }
    else
    {
        currentgamemode(state).activate(state, std::bind(&Map::gotonextgamemode, this, std::placeholders::_1, std::placeholders::_2));
    }
}