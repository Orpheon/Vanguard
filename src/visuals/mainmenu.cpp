#include "visuals/mainmenu.h"
#include "global_constants.h"

#include <SFML/Window/Event.hpp>

#define LEFT_MOUSE_BUTTON 1
#define RIGHT_MOUSE_BUTTON 2

Mainmenu::Mainmenu(sf::RenderWindow &window, MenuContainer &owner_) : Menu(window, owner_), istypingIP(false)
{
    background.init("ui/menus/mainmenu/");

    sf::Font titlefont;
    if (not titlefont.loadFromFile("Vanguard Title Font.ttf"))
    {
        Global::logging().panic(__FILE__, __LINE__, "Vanguard Title Font.ttf not found");
    }
    if (not textfont.loadFromFile("Vanguard Text Font.ttf"))
    {
        Global::logging().panic(__FILE__, __LINE__, "Vanguard Text Font.ttf not found");
    }

    int window_width = window.getSize().x;
    int window_height = window.getSize().y;

    int initial_x = window_width*0.1;
    int initial_height = window_height*0.4;
    double line_spacing = 50;
    int counter = 0;

    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("JOIN SERVER", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         std::bind(&Mainmenu::joinlobby, this),
                                                                         titlefont)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("HOST SERVER", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         std::bind(&Mainmenu::hostserver, this),
                                                                         titlefont)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("JOIN SELF (DEBUG)", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         std::bind(&Mainmenu::joinself, this),
                                                                         titlefont)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("CONNECT MANUALLY", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         std::bind(&Mainmenu::connectmanually, this),
                                                                         titlefont)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("OPTIONS", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         std::bind(&Mainmenu::openoptions, this),
                                                                         titlefont)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("COMMUNITY", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         nullptr, titlefont)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("CREDITS", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         nullptr, titlefont)));
    buttons.push_back(std::unique_ptr<MainmenuButton>(new MainmenuButton("QUIT", initial_x,
                                                                         initial_height + (counter++)*line_spacing,
                                                                         std::bind(&Mainmenu::quit, this),
                                                                         titlefont)));
}

void Mainmenu::run(sf::RenderWindow &window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                quit();
                break;

            case sf::Event::Resized:
                // Nothing - probably placeholder
                break;

            case sf::Event::TextEntered:
                if (istypingIP)
                {
                    // FIXME: Test whether this works
                    ipstring += event.text.unicode;
                }
                break;

            case sf::Event::KeyPressed:
                if (istypingIP)
                {
                    if (event.key.code == sf::Keyboard::Enter)
                    {
                        owner.serverip = ipstring;
                        owner.planned_action = JOIN_SERVER;
                        owner.exitmenus();
                    }
                    else if (event.key.code == sf::Keyboard::Backspace)
                    {
                        if (ipstring.length() > 0)
                        {
                            ipstring.pop_back();
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Escape)
                    {
                        ipstring = "";
                        istypingIP = false;
                    }
                }
                else
                {
                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        quit();
                    }
                }
                break;

            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    for (auto& button : buttons)
                    {
                        if (button->ontop(event.mouseButton.x, event.mouseButton.y))
                        {
                            button->onclick();
                            break;
                        }
                    }
                }
                break;

            default:
                break;
        }
    }

    background.update(MENU_TIMESTEP);

    sf::Vector2i mousepos = sf::Mouse::getPosition(window);
    sf::Vector2u windowsize = window.getSize();

    // Draw
    int window_width = windowsize.x;
    int window_height = windowsize.y;
    sf::Sprite bgsprite;
    spriteloader.loadsprite(background.getframepath(), bgsprite);
    sf::FloatRect size = bgsprite.getLocalBounds();
    bgsprite.setPosition(0, 0);
    bgsprite.setScale(window_width/size.width, window_height/size.height);
    bgsprite.setOrigin(0, 0);
    window.draw(bgsprite);

    for (auto& button : buttons)
    {
        button->render(window, mousepos.x, mousepos.y);
    }

    if (istypingIP)
    {
        sf::Text iptext;
        iptext.setCharacterSize(10);
        iptext.setString("Please enter the server's ip and confirm with enter:");
        iptext.setFont(textfont);
        iptext.setOrigin(0, iptext.getLocalBounds().height * 2.0);
        iptext.setPosition(window_width/2.0, window_height/2.0);
        window.draw(iptext);

        iptext.setString(ipstring);
        iptext.setOrigin(0, 0);
        window.draw(iptext);
    }

    window.display();
}

void Mainmenu::hostserver()
{
    owner.planned_action = POSTMENUACTION::HOST_SERVER;
    owner.exitmenus();
}

void Mainmenu::joinlobby()
{
    owner.planned_action = POSTMENUACTION::OPEN_LOBBY;
}

void Mainmenu::joinself()
{
    owner.planned_action = POSTMENUACTION::JOIN_SERVER;
    owner.serverip = "127.0.0.1";
    owner.serverport = 3226;
    owner.exitmenus();
}

void Mainmenu::connectmanually()
{
    istypingIP = true;
}

void Mainmenu::openoptions()
{
    owner.planned_action = POSTMENUACTION::OPEN_OPTIONS;
}