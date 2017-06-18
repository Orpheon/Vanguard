#include <allegro5/allegro.h>
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
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
    background = al_load_bitmap((mapfolder + bg).c_str());
    wallground = al_load_bitmap((mapfolder + wg).c_str());

    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
    wallmask = al_load_bitmap((mapfolder + wm).c_str());
    al_lock_bitmap(wallmask, al_get_bitmap_format(wallmask), ALLEGRO_LOCK_READONLY);

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
            Rect spawnarea1(gamemode.at("spawn 1")[0], gamemode.at("spawn 1")[1],
                            gamemode.at("spawn 1")[2], gamemode.at("spawn 1")[3]);
            Rect spawnarea2(gamemode.at("spawn 2")[0], gamemode.at("spawn 2")[1],
                            gamemode.at("spawn 2")[2], gamemode.at("spawn 2")[3]);
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
    al_destroy_bitmap(background);
    al_destroy_bitmap(wallground);
    al_unlock_bitmap(wallmask);
    al_destroy_bitmap(wallmask);
}

void Map::renderbackground(Renderer &renderer)
{
    al_draw_scaled_bitmap(background, renderer.cam_x, renderer.cam_y, VIEWPORT_WIDTH, renderer.WINDOW_HEIGHT/renderer.zoom, 0, 0, renderer.WINDOW_WIDTH, renderer.WINDOW_HEIGHT, 0);
}

void Map::renderwallground(Renderer &renderer)
{
    al_draw_scaled_bitmap(wallground, renderer.cam_x, renderer.cam_y, VIEWPORT_WIDTH, renderer.WINDOW_HEIGHT/renderer.zoom, 0, 0, renderer.WINDOW_WIDTH, renderer.WINDOW_HEIGHT, 0);
}

bool Map::collides(Rect r)
{
    if (r.x < 0 or r.y < 0 or r.x+r.w > width() or r.y+r.h > height())
    {
        return true;
    }
    for (int i=0; i<r.w; ++i)
    {
        for (int j=0; j<r.h; ++j)
        {
            if (al_get_pixel(wallmask, i+r.x, j+r.y).a != 0)
            {
                return true;
            }
        }
    }
    return false;
}

bool Map::collides(Gamestate &state, double x, double y, std::string spriteid, double angle)
{
    Rect bbox = state.engine.maskloader.get_rect(spriteid);

    // Check if projectile is outside the map somehow
    if (x + bbox.x > width() or y + bbox.y > height() or x + bbox.x + bbox.w < 0 or y + bbox.y + bbox.h < 0)
    {
        return true;
    }
    // Check pixel-wise
    ALLEGRO_BITMAP *sprite = state.engine.maskloader.requestsprite(spriteid);
//    Global::logging().print(__FILE__, __LINE__, "Flags: %i", al_get_bitmap_flags(sprite));
    double spriteoffset_x = state.engine.maskloader.get_spriteoffset_x(spriteid);
    double spriteoffset_y = state.engine.maskloader.get_spriteoffset_y(spriteid);
    double cosa = std::cos(angle), sina = std::sin(angle);
    for (int i = 0; i < bbox.w; ++i)
    {
        double relx = i - spriteoffset_x;
        for (int j = 0; j < bbox.h; ++j)
        {
            if (al_get_pixel(sprite, i, j).a != 0)
            {
                // Rotate around (x,y) by angle
                double rely = j - spriteoffset_y;
                double rotx = x + cosa*relx - sina*rely;
                double roty = y + sina*relx + cosa*rely;
                if (al_get_pixel(wallmask, rotx, roty).a != 0)
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
    double mapw = width(), maph = height();
    if (x1 < 0 or y1 < 0 or x2 < 0 or y2 < 0 or x1 > mapw or x2 > mapw or y1 > maph or y2 > maph)
    {
        return true;
    }

    int nsteps = std::ceil(std::max(std::abs(x1-x2), std::abs(y1-y2)));
    double dx = static_cast<double>(x2-x1)/nsteps, dy = static_cast<double>(y2-y1)*1.0/nsteps;
    for (int i=0; i<nsteps; ++i)
    {
        if (al_get_pixel(wallmask, x1, y1).a != 0)
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
        Global::logging().print(__FILE__, __LINE__, "Map is over, won by %i", winners);
    }
    else
    {
        currentgamemode(state).activate(state, std::bind(&Map::gotonextgamemode, this, std::placeholders::_1, std::placeholders::_2));
    }
}