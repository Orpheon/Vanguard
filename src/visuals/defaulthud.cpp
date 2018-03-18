
#include "visuals/defaulthud.h"
#include "renderer.h"
#include "colorpalette.h"

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

        renderhealthbar(renderer, state, character);

        // Ult button
        double total_width = 100 * renderer.zoom;
        double mid_x = renderer.WINDOW_WIDTH / 2.0;
        double mid_y = HUD_Y_BASELINE;
        double alpha = 0.6;

        if (myself.ultcharge.active)
        {
            // Inner orange circle
            al_draw_circle(mid_x, mid_y, total_width / 4.0, al_map_rgb(239, 179, 89), total_width / 30.0);
            // Thick gray mid circle
            al_draw_circle(mid_x, mid_y, total_width / 3.0,
                           al_map_rgba(255 * alpha, 255 * alpha, 255 * alpha,255 * alpha), total_width / 14.0);
            // Outer gray circle
            al_draw_circle(mid_x, mid_y, total_width / 2.0,
                           al_map_rgba(255 * alpha, 255 * alpha, 255 * alpha, 255 * alpha), total_width / 30.0);
            // Charging arc
            al_draw_arc(mid_x, mid_y, total_width / 3.0, -PI/2.0, 2*PI * myself.ultcharge.timer/100.0,
                        al_map_rgb(255, 230, 125), total_width / 14.0);
        }
        else
        {
            // Ult symbol
            std::string mainsprite = "ui/ingame/"+character.herofolder()+"ultready";
            ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
            double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
            double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
            al_draw_bitmap(sprite, mid_x - spriteoffset_x, mid_y - spriteoffset_y, 0);
            // Inner gray circle
            al_draw_circle(mid_x, mid_y, total_width / 4.0,
                           al_map_rgba(255 * alpha, 255 * alpha, 255 * alpha, 255 * alpha), total_width / 25.0);
            // Neon blue mid circle
            al_draw_circle(mid_x, mid_y, total_width / 3.0, al_map_rgb(110, 253, 251), total_width / 30.0);
            // Outer gray circle
            al_draw_circle(mid_x, mid_y, total_width / 2.0,
                           al_map_rgba(255 * alpha, 255 * alpha, 255 * alpha, 255 * alpha), total_width / 30.0);
        }
    }
}

void DefaultHud::renderkillfeed(Renderer &renderer, Gamestate &state, Player &myself)
{
    double yoffset = renderer.WINDOW_HEIGHT / 12.0;
    double xborder = renderer.WINDOW_WIDTH * 50.0/51.0;

    for (auto &event : state.killfeed)
    {
        std::string fulltext = event.killername + " killed " + event.victimname;
        double xoffset = al_get_text_width(renderer.font12, fulltext.c_str());
        if (myself.team == event.team)
        {
            al_draw_text(renderer.font12, al_map_rgb(255, 255, 255), xborder - xoffset, yoffset, 0, fulltext.c_str());
        }
        else
        {
            al_draw_text(renderer.font12, al_map_rgb(255, 0, 0), xborder - xoffset, yoffset, 0, fulltext.c_str());
        }

        yoffset += al_get_font_line_height(renderer.font12) + 10;
    }
}

void DefaultHud::renderhealthbar(Renderer &renderer, Gamestate &state, Character &character)
{
    ALLEGRO_COLOR healthcolors[] = { ColorPalette::premul(Color::HP, 255),
                                     ColorPalette::premul(Color::ARMOR, 255),
                                     ColorPalette::premul(Color::TORBARMOR, 255),
                                     ColorPalette::premul(Color::SHIELD, 255),
                                     ColorPalette::premul(Color::SHIELD, 255),
                                     ColorPalette::premul(Color::LUCIOSHIELD, 255),
                                     ColorPalette::premul(Color::HP, 51)};

    double healthamounts[] = { character.hp.normal,
                               character.hp.armor,
                               character.hp.torbarmor,
                               character.hp.shields,
                               character.hp.symshields,
                               character.hp.lucioshields,
                               std::max(character.hp.max()-character.hp.total(), 0.0)};
    int healthamounts_length = sizeof(healthamounts) / sizeof(healthamounts[0]);

    double health_height = renderer.zoom * 15;
    double health_top_y = HUD_Y_BASELINE - health_height / 2.0;
    double totalwidth = renderer.zoom * 200;
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
    float r[8]; // Array used to pass the polygon data for the actual drawing

    for (int rect = 0; rect < nrects; ++rect)
    {
        double rect_x = center_x + (rect - nrects/2.0) * (rect_width)
                        + std::floor(rect - nrects/2.0 + 0.5) * (between_rect_spacing);

        double recthp = hp_per_rect;
        for (int i=0; i<healthamounts_length; ++i)
        {
            double &healthamount = healthamounts[i];
            if (healthamount >= recthp)
            {
                double rectpercentage = (hp_per_rect-recthp) / hp_per_rect;
                // Topleft
                r[0] = rect_x + rect_width * rectpercentage + health_height*slant;
                r[1] = health_top_y;

                // Bottomleft
                r[2] = rect_x + rect_width * rectpercentage;
                r[3] = health_top_y + health_height;

                // Bottomright
                r[4] = rect_x + rect_width;
                r[5] = health_top_y + health_height;

                // Topright
                r[6] = rect_x + rect_width + health_height*slant;
                r[7] = health_top_y;

                al_draw_filled_polygon(r, 4, healthcolors[i]);

                healthamount -= recthp;
                // We're done here, no need to keep this iteration
                break;
            }
            else
            {
                // We need to draw several health types into a single rect
                // This is where it gets fun
                double start_rectpercentage = (hp_per_rect-recthp) / hp_per_rect;
                recthp -= healthamount;
                healthamount = 0;
                double end_rectpercentage = (hp_per_rect-recthp) / hp_per_rect;

                // Topleft
                r[0] = rect_x + rect_width * start_rectpercentage + health_height*slant;
                r[1] = health_top_y;

                // Bottomleft
                r[2] = rect_x + rect_width * start_rectpercentage;
                r[3] = health_top_y + health_height;

                // Bottomright
                r[4] = rect_x + rect_width * end_rectpercentage;
                r[5] = health_top_y + health_height;

                // Topright
                r[6] = rect_x + rect_width * end_rectpercentage + health_height*slant;
                r[7] = health_top_y;

                al_draw_filled_polygon(r, 4, healthcolors[i]);
            }
        }
    }
}

void DefaultHud::mccreehud(Renderer &renderer, Gamestate &state, Mccree &myself)
{
    double abilities_x = renderer.WINDOW_WIDTH * 6.0/7.0;
    double abilities_y = HUD_Y_BASELINE;

    std::string mainsprite = "ui/ingame/"+myself.herofolder()+"weapon";
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    Rect spriterect = renderer.spriteloader.get_rect(mainsprite);
    al_draw_bitmap(sprite, abilities_x, abilities_y - spriterect.h / 2.0, 0);
    // Ammo count
    Peacemaker &weapon = state.get<Peacemaker&>(myself.weapon);
    std::string ammo = std::to_string(weapon.clip);
    std::string maxammo = "I "+std::to_string(weapon.getclipsize());
    double total_ammo_width = al_get_text_width(renderer.font12, ammo.c_str())
                              + al_get_text_width(renderer.font8, maxammo.c_str());
    double space_between_weapon_and_ammo = 10 * renderer.zoom;
    double ammo_x = abilities_x + spriterect.w - total_ammo_width;
    double ammo_y = abilities_y - spriterect.h / 2.0 - space_between_weapon_and_ammo;
    al_draw_text(renderer.font12, al_map_rgb(255, 255, 255), ammo_x, ammo_y - al_get_font_line_height(renderer.font12),
                 ALLEGRO_ALIGN_LEFT, ammo.c_str());
    al_draw_text(renderer.font8, al_map_rgb(255, 255, 255),
                 ammo_x + al_get_text_width(renderer.font12, ammo.c_str()) * 1.1,
                 ammo_y - al_get_font_line_height(renderer.font8), ALLEGRO_ALIGN_LEFT, maxammo.c_str());
    abilities_x -= spriterect.w;

    abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"rolling", abilities_x, abilities_y,
                                 myself.rollcooldown, myself.rollanim.active());
    abilities_x -= renderability(renderer, "ui/ingame/"+myself.herofolder()+"flashbang", abilities_x, abilities_y,
                                 myself.flashbangcooldown, false);
}

void DefaultHud::reinhardthud(Renderer &renderer, Gamestate &state, Reinhardt &myself)
{
    double abilities_x = renderer.WINDOW_WIDTH * 6.0/7.0;
    double abilities_y = HUD_Y_BASELINE;

    std::string mainsprite = "ui/ingame/"+myself.herofolder()+"weapon";
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    Rect spriterect = renderer.spriteloader.get_rect(mainsprite);
    al_draw_bitmap(sprite, abilities_x, abilities_y - spriterect.h / 2.0, 0);
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

    std::string mainsprite = "ui/ingame/"+myself.herofolder()+"weapon";
    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    Rect spriterect = renderer.spriteloader.get_rect(mainsprite);
    al_draw_bitmap(sprite, abilities_x, abilities_y - spriterect.h / 2.0, 0);
    // Ammo count
    Sonicamp &weapon = state.get<Sonicamp&>(myself.weapon);
    std::string ammo = std::to_string(weapon.clip);
    std::string maxammo = "I "+std::to_string(weapon.getclipsize());
    double total_ammo_width = al_get_text_width(renderer.font12, ammo.c_str())
                              + al_get_text_width(renderer.font8, maxammo.c_str());
    double space_between_weapon_and_ammo = 10 * renderer.zoom;
    double ammo_x = abilities_x + spriterect.w - total_ammo_width;
    double ammo_y = abilities_y - spriterect.h / 2.0 - space_between_weapon_and_ammo;
    al_draw_text(renderer.font12, al_map_rgb(255, 255, 255), ammo_x, ammo_y - al_get_font_line_height(renderer.font12),
                 ALLEGRO_ALIGN_LEFT, ammo.c_str());
    al_draw_text(renderer.font8, al_map_rgb(255, 255, 255),
                 ammo_x + al_get_text_width(renderer.font12, ammo.c_str()) * 1.1,
                 ammo_y - al_get_font_line_height(renderer.font8), ALLEGRO_ALIGN_LEFT, maxammo.c_str());
    abilities_x -= renderer.WINDOW_WIDTH * 1.0/10.0;
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

double DefaultHud::renderability(Renderer &renderer, std::string spritename, double x, double y, Timer cooldown,
                                 bool active)
{
    float r[8];

    ALLEGRO_BITMAP *sprite;
    Rect spriterect = renderer.spriteloader.get_rect(spritename);
    if (active)
    {
        sprite = renderer.spriteloader.requestsprite(spritename+"active");
    }
    else if (cooldown.active)
    {
        sprite = renderer.spriteloader.requestsprite(spritename+"cooldown");
    }
    else
    {
        sprite = renderer.spriteloader.requestsprite(spritename);
    }
    spriterect.x = x;
    spriterect.y = y - spriterect.h / 2.0;
    al_draw_bitmap(sprite, spriterect.x, spriterect.y, 0);
    if (cooldown.active and not active)
    {
        // Draw the fill-in
        r[0] = spriterect.x+17*cooldown.getpercent()*renderer.zoom;
        r[1] = spriterect.y+2+34*(1-cooldown.getpercent())*renderer.zoom;

        r[2] = spriterect.x;
        r[3] = spriterect.y+spriterect.h-2;

        r[4] = spriterect.x+41*renderer.zoom;
        r[5] = spriterect.y+spriterect.h-2;

        r[6] = spriterect.x+17*cooldown.getpercent()*renderer.zoom + 41*renderer.zoom;
        r[7] = spriterect.y+2+34*(1-cooldown.getpercent())*renderer.zoom;

        al_draw_filled_polygon(r, 4, al_premul_rgba_f(239/255.0, 179/255.0, 89/255.0, 0.5));

        al_draw_text(renderer.font12, al_map_rgb(255, 255, 255), spriterect.x+spriterect.w/2.0+2,
                     spriterect.y+spriterect.h/2.0-al_get_font_line_height(renderer.font12)/2.0, ALLEGRO_ALIGN_CENTER,
                     std::to_string((int)std::ceil(cooldown.duration - cooldown.timer)).c_str());
    }

    return spriterect.w;
}