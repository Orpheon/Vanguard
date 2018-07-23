#include <cstdio>
#include <vector>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <engine.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
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

Renderer::Renderer() : myself(0), WINDOW_WIDTH(0), WINDOW_HEIGHT(0), spriteloader(false)
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
    myself = myself_;

    if (state.displaystats)
    {
        // Display end of map score

        // Clear black
        window.clear();

        // Draw the map background first
        state.currentmap->renderbackground(*this);

        // Draw translucent black overlay
        sf::RectangleShape rect(sf::Vector2(WINDOW_WIDTH, WINDOW_HEIGHT));
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

        background.clear();
        midground.clear();
        foreground.clear();
        surfaceground.clear();

        background.setView(cameraview);
        midground.setView(cameraview);
        foreground.setView(cameraview);

        // Go through all objects and let them render themselves on the layers
        for (auto &e : state.entitylist)
        {
            if (e.second->isrootobject() and not e.second->destroyentity)
            {
                e.second->render(*this, state);
            }
        }
        background.display();
        midground.display();
        foreground.display();
        surfaceground.display();

        window.clear();
        state.currentmap->renderbackground(*this);
        sf::Sprite sprite(background.getTexture());
        window.draw(sprite);
        sprite.setTexture(midground.getTexture());
        window.draw(sprite);
        sprite.setTexture(foreground.getTexture());
        window.draw(sprite);

        // Draw the map wallmask on top of everything, to prevent sprites that go through walls
        // FIXME: Make this return a thing and render that here, instead of accessing window
        state.currentmap->renderwallground(*this);

        // Draw the final layer on top of even that, for certain things like character healthbars
        sprite.setTexture(surfaceground.getTexture());
        window.draw(sprite);

        // FIXME: This too, should be done here
        if (state.exists(myself))
        {
            currenthud->render(*this, state, state.get<Player>(myself));
        }
    }
    window.display();
}

sf::RenderWindow& Renderer::createnewdisplay()
{
    //default display values are set on header file
    int display_width, display_height, display_type;

    display_width = Global::settings().at("Display resolution").at(0);
    display_height = Global::settings().at("Display resolution").at(1);
    // TODO: Add display settings in config

    sf::RenderWindow display(sf::VideoMode(display_width, display_height), "Vanguard");
    return display;
}