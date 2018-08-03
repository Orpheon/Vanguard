#include <cstdio>
#include <vector>
#include <string>
#include <engine.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "renderer.h"
#include "global_constants.h"
#include "entity.h"
#include "configloader.h"
#include "global.h"
#include "visuals/hud.h"
#include "visuals/defaulthud.h"

Renderer::Renderer() : myself(0), WINDOW_WIDTH(0), WINDOW_HEIGHT(0), spriteloader()
{
    if (!mainfont.loadFromFile("Vanguard Text Font.ttf"))
    {
        Global::logging().panic(__FILE__, __LINE__, "Vanguard font could not be found.");
    }

    WINDOW_WIDTH = Global::settings().at("Display resolution").at(0);
    WINDOW_HEIGHT = Global::settings().at("Display resolution").at(1);

    cameraview.reset(sf::FloatRect(0, 0, VIEWPORT_WIDTH, VIEWPORT_WIDTH*1.0*WINDOW_HEIGHT/WINDOW_WIDTH));

    currenthud = std::unique_ptr<Hud>(new DefaultHud());
}

Renderer::~Renderer()
{
    // Nothing to do
}

void Renderer::render(sf::RenderWindow &window, Gamestate &state, EntityPtr myself_, Networker &networker)
{
    if (state.currentmap->size() != background.getSize())
    {
        resetmapsize(state.currentmap->size());
    }


    myself = myself_;

    if (state.displaystats)
    {
        // Display end of map score

        // Clear black
        window.clear();

        // Draw the map background first
        state.currentmap->renderbackground(*this);

        // Draw translucent black overlay
        sf::RectangleShape rect(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        rect.setPosition(0, 0);
        rect.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(rect);

        // Draw score and shit
        sf::Text text;
        text.setString("MAP OVER / STATISTICS PLACEHOLDER");
        text.setPosition(WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0);
        text.setCharacterSize(20);
        sf::FloatRect r = text.getLocalBounds();
        text.setOrigin(r.left + r.width/2.0, r.top + r.height/2.0);
        window.draw(text);
    }
    else
    {
        // Set camera
        if (state.exists(myself))
        {
            Player &p = state.get<Player>(myself);
            if (state.exists(p.character))
            {
                Character &c = p.getcharacter(state);
                cameraview.setCenter(c.x, c.y);
            }
        }

        // Drawing layers for normal objects
        background.clear();
        midground.clear();
        foreground.clear();
        // Drawing layer for everything that should be visible above the wallmask, in particular HP bars and so on
        surfaceground.clear();
        // Drawing layer for unscaled things like the HUD
        hudground.clear();

        background.setView(cameraview);
        midground.setView(cameraview);
        foreground.setView(cameraview);
        surfaceground.setView(cameraview);

        // Go through all objects and let them render themselves on the layers
        state.currentmap->renderbackground(*this);
        for (auto &e : state.entitylist)
        {
            if (e.second->isrootobject() and not e.second->destroyentity)
            {
                e.second->render(*this, state);
            }
        }
        if (state.exists(myself))
        {
            currenthud->render(*this, state, state.get<Player>(myself));
        }

        background.display();
        midground.display();
        foreground.display();
        surfaceground.display();
        hudground.display();

        window.clear();
        sf::Sprite sprite(background.getTexture());
        window.draw(sprite);
        sprite.setTexture(midground.getTexture());
        window.draw(sprite);
        sprite.setTexture(foreground.getTexture());
        window.draw(sprite);
        sprite.setTexture(surfaceground.getTexture());
        window.draw(sprite);
        sprite.setTexture(hudground.getTexture());
        window.draw(sprite);
    }
    window.display();
}

void Renderer::resetcamera()
{
    sf::Vector2f center = cameraview.getCenter();
    cameraview.reset(sf::FloatRect(center.x, center.y, VIEWPORT_WIDTH, VIEWPORT_WIDTH*1.0*WINDOW_HEIGHT/WINDOW_WIDTH));
}

void Renderer::resetmapsize(sf::Vector2u size)
{
    background.create(size.x, size.y);
    midground.create(size.x, size.y);
    foreground.create(size.x, size.y);
    surfaceground.create(size.x, size.y);
    hudground.create(size.x, size.y);
}