
#include "visuals/defaulthud.h"
#include "renderer.h"
#include "colorpalette.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Text.hpp>

void DefaultHud::render(Renderer &renderer, Gamestate &state, Player &myself)
{
    HUD_Y_BASELINE = renderer.WINDOW_HEIGHT * 9.0/10.0;

    // TODO: Draw gamemode hud

    renderkillfeed(renderer, state, myself);

    if (state.exists(myself.character))
    {
        Character &character = myself.getcharacter(state);
        if (myself.heroclass == MCCREE)
        {
            mccreehud(renderer, state, static_cast<Mccree&>(character));
        }
        else if (myself.heroclass == REINHARDT)
        {
            reinhardthud(renderer, state, static_cast<Reinhardt&>(character));
        }
        else if (myself.heroclass == LUCIO)
        {
            luciohud(renderer, state, static_cast<Lucio&>(character));
        }
        else if (myself.heroclass == SOLDIER76)
        {
            soldier76hud(renderer, state, static_cast<Soldier76&>(character));
        }

        renderhealthbar(renderer, state, character);

        // Ult button
        double total_width = 100;
        double mid_x = renderer.WINDOW_WIDTH / 2.0;
        double mid_y = HUD_Y_BASELINE;
        double alpha = 0.6;

        sf::CircleShape circle;
        circle.setPosition(mid_x, mid_y);

        if (myself.ultcharge.active)
        {
            // Inner orange circle
            circle.setOutlineColor(sf::Color(239, 179, 89, 255));
            circle.setOutlineThickness(total_width / 30.0);
            circle.setRadius(total_width / 4.0);
            circle.setOrigin(circle.getLocalBounds().width / 2.0, circle.getLocalBounds().height / 2.0);
            renderer.hudground.draw(circle);

            // Thick gray mid circle
            circle.setOutlineColor(sf::Color(255, 255, 255, 255*alpha));
            circle.setOutlineThickness(total_width / 14.0);
            circle.setRadius(total_width / 3.0);
            circle.setOrigin(circle.getLocalBounds().width / 2.0, circle.getLocalBounds().height / 2.0);
            renderer.hudground.draw(circle);

            // Outer gray circle
            circle.setOutlineColor(sf::Color(255, 255, 255, 255*alpha));
            circle.setOutlineThickness(total_width / 30.0);
            circle.setRadius(total_width / 2.0);
            circle.setOrigin(circle.getLocalBounds().width / 2.0, circle.getLocalBounds().height / 2.0);
            renderer.hudground.draw(circle);

            // Charging arc
            // TODO: Reimplement this using sfml, or find workaround, or something
//            al_draw_arc(mid_x, mid_y, total_width / 3.0, -PI/2.0, 2*PI * myself.ultcharge.timer/100.0,
//                        al_map_rgb(255, 230, 125), total_width / 14.0);
        }
        else
        {
            // Ult symbol
            std::string spritepath = "ui/ingame/"+character.herofolder()+"ultready";
            sf::Sprite sprite;
            renderer.spriteloader.loadsprite(spritepath, sprite);
            renderer.hudground.draw(sprite);

            // Inner gray circle
            circle.setOutlineColor(sf::Color(255, 255, 255, 255*alpha));
            circle.setOutlineThickness(total_width / 25.0);
            circle.setRadius(total_width / 4.0);
            circle.setOrigin(circle.getLocalBounds().width / 2.0, circle.getLocalBounds().height / 2.0);
            renderer.hudground.draw(circle);

            // Neon blue mid circle
            circle.setOutlineColor(sf::Color(110, 253, 251, 255*alpha));
            circle.setOutlineThickness(total_width / 30.0);
            circle.setRadius(total_width / 3.0);
            circle.setOrigin(circle.getLocalBounds().width / 2.0, circle.getLocalBounds().height / 2.0);
            renderer.hudground.draw(circle);

            // Outer gray circle
            circle.setOutlineColor(sf::Color(255, 255, 255, 255*alpha));
            circle.setOutlineThickness(total_width / 30.0);
            circle.setRadius(total_width / 2.0);
            circle.setOrigin(circle.getLocalBounds().width / 2.0, circle.getLocalBounds().height / 2.0);
            renderer.hudground.draw(circle);
        }
    }
}

void DefaultHud::renderkillfeed(Renderer &renderer, Gamestate &state, Player &myself)
{
    double yoffset = renderer.WINDOW_HEIGHT / 12.0;
    double xborder = renderer.WINDOW_WIDTH * 50.0/51.0;

    for (auto &event : state.killfeed)
    {
        // FIXME: This is a really ugly placeholder, loop contents should be replaced
        // I'm too lazy to port a placeholder, might as well do it properly

//        std::string fulltext = event.killername + " killed " + event.victimname;
//        if (event.killtype == SUICIDE)
//        {
//            fulltext = event.victimname + " suicided";
//        }
//        double xoffset = al_get_text_width(renderer.font12, fulltext.c_str());
//        if (myself.team == event.team)
//        {
//            al_draw_text(renderer.font12, al_map_rgb(255, 255, 255), xborder - xoffset, yoffset, 0, fulltext.c_str());
//        }
//        else
//        {
//            al_draw_text(renderer.font12, al_map_rgb(255, 0, 0), xborder - xoffset, yoffset, 0, fulltext.c_str());
//        }
//
//        yoffset += al_get_font_line_height(renderer.font12) + 10;
    }
}

void DefaultHud::renderhealthbar(Renderer &renderer, Gamestate &state, Character &character)
{
    sf::Color healthcolors[] = {
        COLOR_HP,
        COLOR_ARMOR,
//        COLOR_TORBARMOR,
        COLOR_SHIELD,
        COLOR_SHIELD,
        COLOR_LUCIOSHIELD,
        sf::Color(COLOR_HP.r, COLOR_HP.g, COLOR_HP.b, 51)
    };

    double healthamounts[] = { character.hp.normal,
                               character.hp.armor,
                               character.hp.torbarmor,
                               character.hp.shields,
                               character.hp.symshields,
                               character.hp.lucioshields,
                               std::max(character.hp.max()-character.hp.total(), 0.0)};
    int healthamounts_length = sizeof(healthamounts) / sizeof(healthamounts[0]);

    double health_height = 15;
    double health_top_y = HUD_Y_BASELINE - health_height / 2.0;
    double totalwidth = 200;
    double center_x = renderer.WINDOW_WIDTH/11.0 + totalwidth/2.0;
    double between_rect_spacing = 2;
    double slant = 0.3;
    double hp_per_rect = 25;

    int nrects = static_cast<int>(std::ceil((character.hp.max()+character.hp.torbarmor+character.hp.lucioshields)
                                            / hp_per_rect));
    double rect_width = (totalwidth - between_rect_spacing*(nrects-1)) / nrects;
    if (rect_width < 1)
    {
        // If we have a lot of health on a small window, remove spacing and make a continuous line
        between_rect_spacing = 0;
        rect_width = totalwidth / nrects;
    }

    sf::ConvexShape slanted_rect;
    slanted_rect.setPointCount(4);
    for (int rect = 0; rect < nrects; ++rect)
    {
        double rect_x = center_x + (rect - nrects/2.0) * (rect_width)
                        + std::floor(rect - nrects/2.0 + 0.5) * (between_rect_spacing);

        double recthp = hp_per_rect;
        for (int i=0; i<healthamounts_length; ++i)
        {
            double &healthamount = healthamounts[i];
            slanted_rect.setFillColor(healthcolors[i]);
            if (healthamount >= recthp)
            {
                double rectpercentage = (hp_per_rect-recthp) / hp_per_rect;
                // Topleft
                slanted_rect.setPoint(0, sf::Vector2f(rect_x + rect_width * rectpercentage + health_height*slant,
                                                      health_top_y));
                // Bottomleft
                slanted_rect.setPoint(1, sf::Vector2f(rect_x + rect_width * rectpercentage,
                                                      health_top_y + health_height));
                // Bottomright
                slanted_rect.setPoint(2, sf::Vector2f(rect_x + rect_width,
                                                      health_top_y + health_height));
                // Topright
                slanted_rect.setPoint(3, sf::Vector2f(rect_x + rect_width + health_height*slant,
                                                      health_top_y));

                renderer.hudground.draw(slanted_rect);

                healthamount -= recthp;
                // We're done here, no need to keep this iteration
                break;
            }
            else
            {
                // TODO:
//                Global::logging().print(__FILE__, __LINE__, "Rendering mixed-health rects is currently not working, don't want to try implementing without being able to test.");
                // This code below can't work, it only seems to draw the first half without caring about the second
//                // We need to draw several health types into a single rect
//                // This is where it gets fun
//                double start_rectpercentage = (hp_per_rect-recthp) / hp_per_rect;
//                recthp -= healthamount;
//                healthamount = 0;
//                double end_rectpercentage = (hp_per_rect-recthp) / hp_per_rect;
//
//                // Topleft
//                r[0] = rect_x + rect_width * start_rectpercentage + health_height*slant;
//                r[1] = health_top_y;
//
//                // Bottomleft
//                r[2] = rect_x + rect_width * start_rectpercentage;
//                r[3] = health_top_y + health_height;
//
//                // Bottomright
//                r[4] = rect_x + rect_width * end_rectpercentage;
//                r[5] = health_top_y + health_height;
//
//                // Topright
//                r[6] = rect_x + rect_width * end_rectpercentage + health_height*slant;
//                r[7] = health_top_y;
//
//                al_draw_filled_polygon(r, 4, healthcolors[i]);
            }
        }
    }
}

void DefaultHud::mccreehud(Renderer &renderer, Gamestate &state, Mccree &myself)
{
    double abilities_x = renderer.WINDOW_WIDTH * 6.0/7.0;
    double abilities_y = HUD_Y_BASELINE;

    // Weapon icon
    std::string spritepath = "ui/ingame/"+myself.herofolder()+"weapon";
    sf::Sprite sprite;
    renderer.spriteloader.loadsprite(spritepath, sprite);
    sprite.setPosition(abilities_x, abilities_y);
    renderer.hudground.draw(sprite);

    // Ammo count
    Peacemaker &weapon = state.get<Peacemaker&>(myself.weapon);
    std::string ammo = std::to_string(weapon.clip);
    std::string maxammo = "I "+std::to_string(weapon.getclipsize());

    // TODO when I can see things
//    double total_ammo_width = al_get_text_width(renderer.font12, ammo.c_str())
//                              + al_get_text_width(renderer.font8, maxammo.c_str());
//    double space_between_weapon_and_ammo = 10 * renderer.zoom;
//    double ammo_x = abilities_x + spriterect.w - total_ammo_width;
//    double ammo_y = abilities_y - spriterect.h / 2.0 - space_between_weapon_and_ammo;
//    al_draw_text(renderer.font12, al_map_rgb(255, 255, 255), ammo_x, ammo_y - al_get_font_line_height(renderer.font12),
//                 ALLEGRO_ALIGN_LEFT, ammo.c_str());
//    al_draw_text(renderer.font8, al_map_rgb(255, 255, 255),
//                 ammo_x + al_get_text_width(renderer.font12, ammo.c_str()) * 1.1,
//                 ammo_y - al_get_font_line_height(renderer.font8), ALLEGRO_ALIGN_LEFT, maxammo.c_str());
//    abilities_x -= spriterect.w;

    abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"rolling", abilities_x, abilities_y,
                                 myself.rollcooldown, myself.rollanim.active());
    abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"flashbang", abilities_x, abilities_y,
                                 myself.flashbangcooldown, false);
}

void DefaultHud::reinhardthud(Renderer &renderer, Gamestate &state, Reinhardt &myself)
{
    double abilities_x = renderer.WINDOW_WIDTH * 6.0/7.0;
    double abilities_y = HUD_Y_BASELINE;

    // Weapon icon
    std::string spritepath = "ui/ingame/"+myself.herofolder()+"weapon";
    sf::Sprite sprite;
    renderer.spriteloader.loadsprite(spritepath, sprite);
    sprite.setPosition(abilities_x, abilities_y);
    renderer.hudground.draw(sprite);

    abilities_x -= renderer.WINDOW_WIDTH * 1.0/10.0;

    Hammer& hammer = state.get<Hammer&>(myself.weapon);

    abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"firestrike", abilities_x, abilities_y,
                                 hammer.firestrikecooldown, hammer.firestrikeanim.active());
    abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"charge", abilities_x, abilities_y,
                                 myself.chargecooldown, myself.preparechargeanim.active()
                                                        or myself.chargeanim.active());
    abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"shield", abilities_x, abilities_y,
                                 hammer.barrier(state).brokencooldown, hammer.barrier(state).active);
}

void DefaultHud::luciohud(Renderer &renderer, Gamestate &state, Lucio &myself)
{
    double abilities_x = renderer.WINDOW_WIDTH * 6.0/7.0;
    double abilities_y = HUD_Y_BASELINE;

    // Weapon icon
    std::string spritepath = "ui/ingame/"+myself.herofolder()+"weapon";
    sf::Sprite sprite;
    renderer.spriteloader.loadsprite(spritepath, sprite);
    sprite.setPosition(abilities_x, abilities_y);
    renderer.hudground.draw(sprite);

    // TODO: Do this when I can see stuff
//    // Ammo count
    Sonicamp &weapon = state.get<Sonicamp&>(myself.weapon);
//    std::string ammo = std::to_string(weapon.clip);
//    std::string maxammo = "I "+std::to_string(weapon.getclipsize());
//    double total_ammo_width = al_get_text_width(renderer.font12, ammo.c_str())
//                              + al_get_text_width(renderer.font8, maxammo.c_str());
//    double space_between_weapon_and_ammo = 10 * renderer.zoom;
//    double ammo_x = abilities_x + spriterect.w - total_ammo_width;
//    double ammo_y = abilities_y - spriterect.h / 2.0 - space_between_weapon_and_ammo;
//    al_draw_text(renderer.font12, al_map_rgb(255, 255, 255), ammo_x, ammo_y - al_get_font_line_height(renderer.font12),
//                 ALLEGRO_ALIGN_LEFT, ammo.c_str());
//    al_draw_text(renderer.font8, al_map_rgb(255, 255, 255),
//                 ammo_x + al_get_text_width(renderer.font12, ammo.c_str()) * 1.1,
//                 ammo_y - al_get_font_line_height(renderer.font8), ALLEGRO_ALIGN_LEFT, maxammo.c_str());
//    abilities_x -= renderer.WINDOW_WIDTH * 1.0/10.0;
    Timer emptytimer;
    emptytimer.init(0, false);

    abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"soundwave", abilities_x, abilities_y,
                                 weapon.soundwavecooldown, weapon.soundwave.active());
    abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"ampitup", abilities_x, abilities_y,
                                 myself.ampitupcooldown, myself.ampitup.active);
    if (myself.crossfadespeed.active())
    {
        abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"crossfadetogglespeed", abilities_x,
                                     abilities_y, emptytimer, true);
    }
    else if (myself.crossfadeheal.active())
    {
        abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"crossfadetoggleheal", abilities_x,
                                     abilities_y, emptytimer, true);
    }
    else if (myself.currentaura == myself.HEALAURA)
    {
        abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"crossfadetogglespeed", abilities_x,
                                     abilities_y, emptytimer, false);
    }
    else
    {
        abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"crossfadetoggleheal", abilities_x,
                                     abilities_y, emptytimer, false);
    }

}

void DefaultHud::soldier76hud(Renderer &renderer, Gamestate &state, Soldier76 &myself)
{
    // TODO
}

double DefaultHud::renderability(Renderer &renderer, std::string spritename, double x, double y, Timer cooldown,
                                 bool active)
{
    sf::Sprite sprite;
    if (active)
    {
        renderer.spriteloader.loadsprite(spritename + "active", sprite);
    }
    else if (cooldown.active)
    {
        renderer.spriteloader.loadsprite(spritename + "cooldown", sprite);
    }
    else
    {
        renderer.spriteloader.loadsprite(spritename, sprite);
    }
    sprite.setPosition(x, y);
    renderer.hudground.draw(sprite);

    sf::FloatRect spriterect = sprite.getGlobalBounds();

    // Cooldown overlay
    if (cooldown.active and not active)
    {
        sf::ConvexShape slanted_rect;
        slanted_rect.setPointCount(4);
        slanted_rect.setFillColor(sf::Color(239, 179, 89, 255*0.5));
        slanted_rect.setPoint(0, sf::Vector2f(spriterect.left + 17*cooldown.getpercent(),
                                              spriterect.top + 2 + 34*(1 - cooldown.getpercent())));
        slanted_rect.setPoint(1, sf::Vector2f(spriterect.left,
                                              spriterect.top + spriterect.height - 2));
        slanted_rect.setPoint(2, sf::Vector2f(spriterect.left + 41,
                                              spriterect.top + spriterect.height - 2));
        slanted_rect.setPoint(3, sf::Vector2f(spriterect.left + 17*cooldown.getpercent() + 41,
                                              spriterect.top + 2 + 34*(1 - cooldown.getpercent())));
        renderer.hudground.draw(slanted_rect);

        sf::Text text;
        text.setFont(renderer.mainfont);
        text.setCharacterSize(12);
        text.setString(std::to_string((int)std::ceil(cooldown.duration - cooldown.timer)));
        text.setOrigin(text.getLocalBounds().width/2.0, text.getLocalBounds().height);
        text.setPosition(spriterect.left + spriterect.width/2.0, spriterect.top + spriterect.height/2.0);
        renderer.hudground.draw(text);
    }

    return spriterect.width;
}