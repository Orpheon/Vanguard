
#include "visuals/defaulthud.h"
#include "renderer.h"
#include "colorpalette.h"

void DefaultHud::render(Renderer &renderer, Gamestate &state, Player &myself)
{
    double HUD_Y_BASELINE = renderer.WINDOW_HEIGHT * 9.0/10.0;

    // TODO: Draw gamemode hud

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

        // --------------- HEALTHBAR ---------------
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
        double health_top_y = HUD_Y_BASELINE - health_height;
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
        // --------------- /HEALTHBAR ---------------


        // Ult button
        double total_width = 100 * renderer.zoom;
        double mid_x = renderer.WINDOW_WIDTH / 2.0;
        double mid_y = HUD_Y_BASELINE - total_width / 3.0;
        double alpha = 0.6;

        if (myself.ultcharge.active)
        {
            // Inner orange circle
            al_draw_circle(mid_x, mid_y, total_width / 4.0, al_map_rgb(239, 179, 89), total_width / 30.0);
            // Thick gray mid circle
            al_draw_circle(mid_x, mid_y, total_width / 3.0, al_map_rgba(255 * alpha, 255 * alpha, 255 * alpha, 255 * alpha),
                           total_width / 14.0);
            // Outer gray circle
            al_draw_circle(mid_x, mid_y, total_width / 2.0, al_map_rgba(255 * alpha, 255 * alpha, 255 * alpha, 255 * alpha),
                           total_width / 30.0);
            // Charging arc
            al_draw_arc(mid_x, mid_y, total_width / 3.0, -3.1415/2.0, 2*3.1415 * myself.ultcharge.timer/100.0,
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
            al_draw_circle(mid_x, mid_y, total_width / 4.0, al_map_rgba(255 * alpha, 255 * alpha, 255 * alpha, 255 * alpha),
                           total_width / 25.0);
            // Neon blue mid circle
            al_draw_circle(mid_x, mid_y, total_width / 3.0, al_map_rgb(110, 253, 251), total_width / 30.0);
            // Outer gray circle
            al_draw_circle(mid_x, mid_y, total_width / 2.0, al_map_rgba(255 * alpha, 255 * alpha, 255 * alpha, 255 * alpha),
                           total_width / 30.0);
        }
    }
}

void DefaultHud::mccreehud(Renderer &renderer, Gamestate &state, Mccree &myself)
{
    double abilities_x = renderer.WINDOW_WIDTH * 5.0/6.0;
    double abilities_y = renderer.WINDOW_HEIGHT* 9.0/10.0;
    double space = renderer.WINDOW_WIDTH / 15.0;
    renderability(renderer, "ui/ingame/"+myself.herofolder()+"rolling", abilities_x, abilities_y, myself.rollcooldown);
    renderability(renderer, "ui/ingame/"+myself.herofolder()+"flashbang", abilities_x - space, abilities_y,
                  myself.flashbangcooldown);
}

void DefaultHud::reinhardthud(Renderer &renderer, Gamestate &state, Reinhardt &myself)
{
    // TODO: Reinhardt hud
}

void DefaultHud::renderability(Renderer &renderer, std::string spritename, double x, double y, Timer cooldown)
{
    float r[8];

    ALLEGRO_BITMAP *sprite;
    Rect spriterect = renderer.spriteloader.get_rect(spritename);
    if (cooldown.active)
    {
        sprite = renderer.spriteloader.requestsprite(spritename+"cooldown");
    }
    else
    {
        sprite = renderer.spriteloader.requestsprite(spritename);
    }
    spriterect.x = x;
    spriterect.y = y - spriterect.h;
    al_draw_bitmap(sprite, spriterect.x, spriterect.y, 0);
    if (cooldown.active)
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

        al_draw_text(renderer.font10, al_map_rgb(255, 255, 255), spriterect.x+spriterect.w/2.0+2,
                     spriterect.y+spriterect.h/2.0-al_get_font_line_height(renderer.font10)/2.0, ALLEGRO_ALIGN_CENTER,
                     std::to_string((int)std::ceil(cooldown.duration - cooldown.timer)).c_str());
    }
}