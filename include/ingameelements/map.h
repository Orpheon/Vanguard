#pragma once

#include "datastructures.h"
#include "ingameelements/movingentity.h"
#include "ingameelements/character.h"
#include "json.hpp"
#include "ingameelements/gamemodes/gamemodemanager.h"
#include "mapelements/spawnroom.h"

#include <SFML/Graphics.hpp>
#include <string>
#include <list>

class Map
{
    public:
        Map(Gamestate &state, std::string name);
        virtual ~Map();
        void renderbackground(Renderer &renderer);
        void renderwallground(Renderer &renderer);
        sf::Vector2u size() { return wallmask.getSize(); }
        bool collides(Rect rect);
        bool collides(Gamestate &state, double x, double y, std::string spriteid, double angle);
        bool collideline(double x1, double y1, double x2, double y2);
        bool testpixel(double x, double y);
        Spawnroom& spawnroom(Gamestate &state, Team team);
        GamemodeManager& currentgamemode(Gamestate &state);
        void gotonextgamemode(Gamestate &state, Team winners);

        std::string name;
    protected:
    private:
        // Textures live on gfx card for rendering, Images live in cpu memory
        sf::Texture background;
        sf::Texture wallground;
        sf::Image wallmask;
        nlohmann::json mapdata;
        std::list<EntityPtr> gamemodes;
};

