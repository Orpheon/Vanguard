#include <cstdio>
#include <fstream>

#include <SFML/Window/Event.hpp>

#include "inputcatcher.h"
#include "datastructures.h"
#include "renderer.h"
#include "networking/clientnetworker.h"
#include "ingameelements/character.h"
#include "global_constants.h"
#include "global.h"

InputCatcher::InputCatcher()
{
    std::ifstream configfile("config.json");
    config << configfile;
    configfile.close();
}

InputCatcher::~InputCatcher()
{

}

void InputCatcher::run(sf::RenderWindow &window, Gamestate &state, Networker &networker, Renderer &renderer,
                       EntityPtr myself)
{
    InputContainer heldkeys;
    heldkeys.reset();

    Player &player = state.get<Player>(myself);

    sf::Event event;
    // Go through all events that have been stacking up since last time
    while (window.pollEvent(event))
    {
        // FIXME: This newclass thing is an ugly placeholder,
        // when you replace it with a real class menu don't do it like this
        Heroclass newclass = player.heroclass;
        switch (event.type)
        {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::Resized:
                renderer.WINDOW_WIDTH = event.size.width;
                renderer.WINDOW_HEIGHT = event.size.height;
                renderer.resetcamera();
                break;

            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;

                    case sf::Keyboard::F9:
                        // Zooming in for sprite analysis purposes
                        if (renderer.VIEWPORT_WIDTH != 960)
                        {
                            renderer.VIEWPORT_WIDTH = 960;
                        }
                        else
                        {
                            renderer.VIEWPORT_WIDTH = 300;
                        }
                        renderer.resetcamera();

                    case sf::Keyboard::Num1:
                        newclass = MCCREE;
                        break;

                    case sf::Keyboard::Num2:
                        newclass = REINHARDT;
                        break;

                    case sf::Keyboard::Num3:
                        newclass = LUCIO;
                        break;

                    default:
                        break;
                }
                if (newclass != player.heroclass)
                {
                    // Player desires a class change
                    if (state.engine.isserver)
                    {
                        player.changeclass(state, newclass);
                        networker.sendbuffer.write<uint8_t>(PLAYER_CHANGECLASS);
                        networker.sendbuffer.write<uint8_t>(state.findplayerid(player.id));
                        networker.sendbuffer.write<uint8_t>(static_cast<uint8_t>(newclass));
                    }
                    else
                    {
                        networker.sendbuffer.write<uint8_t>(PLAYER_CHANGECLASS);
                        networker.sendbuffer.write<uint8_t>(static_cast<uint8_t>(newclass));
                    }
                }
                break;

            default:
                // Ignore other events
                break;
        }
    }

    if (sf::Keyboard::isKeyPressed(config.at("jump")) or sf::Keyboard::isKeyPressed(config.at("jump_alt1"))
        or sf::Keyboard::isKeyPressed(config.at("jump")))
    {
        heldkeys.JUMP = true;
    }
    if (sf::Keyboard::isKeyPressed(config.at("crouch")) or sf::Keyboard::isKeyPressed(config.at("crouch_alt1"))
        or sf::Keyboard::isKeyPressed(config.at("crouch")))
    {
        heldkeys.CROUCH = true;
    }
    if (sf::Keyboard::isKeyPressed(config.at("left")) or sf::Keyboard::isKeyPressed(config.at("left_alt1"))
        or sf::Keyboard::isKeyPressed(config.at("left")))
    {
        heldkeys.LEFT = true;
    }
    if (sf::Keyboard::isKeyPressed(config.at("right")) or sf::Keyboard::isKeyPressed(config.at("right_alt1"))
        or sf::Keyboard::isKeyPressed(config.at("right")))
    {
        heldkeys.RIGHT = true;
    }
    if (sf::Keyboard::isKeyPressed(config.at("ability1")) or sf::Keyboard::isKeyPressed(config.at("ability1_alt1"))
        or sf::Keyboard::isKeyPressed(config.at("ability1")))
    {
        heldkeys.ABILITY_1 = true;
    }
    if (sf::Keyboard::isKeyPressed(config.at("ability2")) or sf::Keyboard::isKeyPressed(config.at("ability2_alt1"))
        or sf::Keyboard::isKeyPressed(config.at("ability2")))
    {
        heldkeys.ABILITY_2 = true;
    }
    if (sf::Keyboard::isKeyPressed(config.at("ultimate")) or sf::Keyboard::isKeyPressed(config.at("ultimate_alt1"))
        or sf::Keyboard::isKeyPressed(config.at("ultimate")))
    {
        heldkeys.ULTIMATE = true;
    }
    if (sf::Keyboard::isKeyPressed(config.at("reload")) or sf::Keyboard::isKeyPressed(config.at("reload_alt1"))
        or sf::Keyboard::isKeyPressed(config.at("reload")))
    {
        heldkeys.RELOAD = true;
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        heldkeys.PRIMARY_FIRE = true;
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        heldkeys.SECONDARY_FIRE = true;
    }

    if (state.exists(player.character))
    {
        Character &c = player.getcharacter(state);

        sf::Vector2f mousepos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        // Set the input for our current character
        c.setinput(state, heldkeys, mousepos.x, mousepos.y);

        // If this is a client, send the input off to the server
        if (not state.engine.isserver)
        {
            ClientNetworker &n = reinterpret_cast<ClientNetworker&>(networker);
            n.sendinput(heldkeys, mousepos.x, mousepos.y);
        }
    }
}
