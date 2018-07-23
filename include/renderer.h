#pragma once

#include "gamestate.h"
#include "spriteloader.h"
#include "networking/networker.h"
#include "visuals/hud.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

class Renderer
{
    public:
        Renderer();
        virtual ~Renderer();
        void render(sf::RenderWindow &window, Gamestate &state, EntityPtr myself_, Networker &networker);
        sf::RenderWindow& createnewdisplay();
        EntityPtr myself;
        int WINDOW_WIDTH;
        int WINDOW_HEIGHT;
        int VIEWPORT_WIDTH = 960;
        sf::View cameraview;
        sf::RenderTexture background;
        sf::RenderTexture midground;
        sf::RenderTexture foreground;
        sf::RenderTexture surfaceground;
        Spriteloader spriteloader;
        sf::Font mainfont;
    protected:
    private:
        sf::Clock fpsclock;
        std::unique_ptr<Hud> currenthud;
};
