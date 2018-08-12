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

void InputCatcher::updatekey(std::string keylabel, bool &key)
{
    int code1 = config.at(keylabel);
    int code2 = config.at(keylabel+"_alt1");
    int code3 = config.at(keylabel+"_alt2");

    if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(code1))
        or sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(code2))
        or sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(code3)))
    {
        key = true;
    }
    else
    {
        key = false;
    }
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
                Global::logging().panic(__FILE__, __LINE__, "Game closed.");
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
                        Global::logging().panic(__FILE__, __LINE__, "Game closed.");
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
                        break;

                    case sf::Keyboard::Num1:
                        newclass = MCCREE;
                        break;

                    case sf::Keyboard::Num2:
                        newclass = REINHARDT;
                        break;

                    case sf::Keyboard::Num3:
                        newclass = LUCIO;
                        break;

                    case sf::Keyboard::Num4:
                        newclass = SOLDIER76;
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
    
    updatekey("jump", heldkeys.JUMP);
    updatekey("crouch", heldkeys.CROUCH);
    updatekey("left", heldkeys.LEFT);
    updatekey("right", heldkeys.RIGHT);
    updatekey("ability1", heldkeys.ABILITY_1);
    updatekey("ability2", heldkeys.ABILITY_2);
    updatekey("ultimate", heldkeys.ULTIMATE);
    updatekey("reload", heldkeys.RELOAD);

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

        sf::Vector2f mousepos = window.mapPixelToCoords(sf::Mouse::getPosition(window), renderer.background.getView());

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
