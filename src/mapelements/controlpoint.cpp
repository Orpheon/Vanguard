#include "mapelements/controlpoint.h"
#include "gamestate.h"
#include "renderer.h"
#include "colorpalette.h"

#include "ingameelements/gamemodes/controlmanager.h"

#include <allegro5/allegro_primitives.h>

void ControlPoint::init(uint64_t id_, Rect area_, uint8_t cpindex_)
{
    Entity::init(id_);
    area = area_;
    cpindex = cpindex_;
    timer.init(5, std::bind(&ControlPoint::unlock, this));
}

void ControlPoint::beginstep(Gamestate &state, double frametime)
{
    if (!locked)
    {
        bool bothteamonpoint = false;
        uint8_t teampresence[2];
        teampresence[TEAM1] = 0;
        teampresence[TEAM2] = 0;

        for (auto pptr : state.playerlist)
        {
            Player &p = state.get<Player>(pptr);
            if (not state.exists(p.character))
            {
                Character &c = p.getcharacter(state);
                if (isinside(c.x, c.y))
                {
                    teampresence[p.team] += 1;
                }
            }
        }

        if (teampresence[TEAM1] and teampresence[TEAM2])
        {
            bothteamonpoint = true;
        }

        Team presenceteam;
        if (teampresence[TEAM1] or teampresence[TEAM2])
        {
            if (bothteamonpoint)
            {
                presenceteam = SPECTATOR;
            }
            else
            {
                if      (teampresence[TEAM1] > 0) { presenceteam = TEAM1; }
                else if (teampresence[TEAM2] > 0) { presenceteam = TEAM2; }
                else                              { presenceteam = SPECTATOR; }
            }
        }
        else
        {
            presenceteam = SPECTATOR;
        }

        if (bothteamonpoint)
        {
            cpidletimer = 0;
            // TODO: Make HUD to show "Contesting"
        }
        else
        {
            if (presenceteam != cappingteam and presenceteam != SPECTATOR)
            {
                cpidletimer = 0;
                if (cappedamount[1 - presenceteam] > 0)
                {
                    cappedamount[1 - presenceteam] -= 5 * frametime * teampresence[presenceteam];
                }
                else
                {
                    cappedamount[presenceteam] += 5 * frametime * teampresence[presenceteam];
                }
                if (cappedamount[presenceteam] >= maxcap)
                {
                    capture(presenceteam, state.gamemodemanager);
                }
            }
            else
            {
                // If no enemy is on point, decrease the capped rate
                if (cpidletimer >= 5.0)
                {
                    if (cappingteam != SPECTATOR)
                    {
                        cappedamount[TEAM1] = std::max(cappedamount[TEAM1] - 5 * frametime,
                                                            std::floor(3.0*cappedamount[TEAM1] / maxcap)*maxcap / 3.0);
                        cappedamount[TEAM2] = std::max(cappedamount[TEAM2] - 5 * frametime,
                                                            std::floor(3.0*cappedamount[TEAM2] / maxcap)*maxcap / 3.0);
                    }
                    else
                    {
                        cappedamount[TEAM1] = std::max(cappedamount[TEAM1] - 5 * frametime, 0.0);
                        cappedamount[TEAM2] = std::max(cappedamount[TEAM2] - 5 * frametime, 0.0);
                    }
                }
                else
                {
                    cpidletimer += frametime;
                }
            }
        }
    }
}

void ControlPoint::midstep(Gamestate &state, double frametime)
{
    timer.update(state, frametime);
}

void ControlPoint::render(Renderer &renderer, Gamestate &state)
{
    // Find the center of CP area
    double rel_x = (area.x + area.w/2 - renderer.cam_x)*renderer.zoom;
    double rel_y = (area.y + area.h/2 - renderer.cam_y)*renderer.zoom;
    Color cpcolor;
    
    if (cappingteam == SPECTATOR)
    {
        cpcolor = Color::CP;
    }
    else if (state.get<Player>(renderer.myself).team != cappingteam)
    {
        cpcolor = Color::ENEMY;
    }
    else
    {
        cpcolor = Color::ALLY;
    }

    ALLEGRO_COLOR cpColor_front = ColorPalette::premul(cpcolor, 200);
    ALLEGRO_COLOR cpColor_middle = ColorPalette::premul(cpcolor, 160);
    ALLEGRO_COLOR cpColor_back = ColorPalette::premul(cpcolor, 80);

    // Draw CP Area
    al_set_target_bitmap(renderer.surfaceground);
    al_draw_line(rel_x - (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        rel_x + (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        cpColor_front, 5);
    al_draw_line(rel_x - (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        rel_x + (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        cpColor_middle, 3);

    al_set_target_bitmap(renderer.background);
    al_draw_line(rel_x - (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        rel_x - (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 20)*renderer.zoom,
        cpColor_middle, 3);
    al_draw_line(rel_x + (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        rel_x + (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 20)*renderer.zoom,
        cpColor_middle, 3);

    al_draw_line(rel_x - (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 20)*renderer.zoom,
        rel_x + (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 20)*renderer.zoom,
        cpColor_back, 3);

    // Draw CP Bubble 
    al_draw_filled_circle(rel_x, rel_y, 30.0, cpColor_front);
    al_draw_circle(rel_x, rel_y, 30.0, cpColor_front, 5);
    al_draw_text(renderer.font20, ColorPalette::get(Color::WHITE), rel_x + 5, rel_y - 20, ALLEGRO_ALIGN_CENTER, "A");
    al_draw_text(renderer.font20, ColorPalette::get(Color::WHITE), rel_x + 5, rel_y + 20, ALLEGRO_ALIGN_CENTER, std::to_string(cappedamount[TEAM1]).c_str());
    al_draw_text(renderer.font20, ColorPalette::get(Color::WHITE), rel_x + 5, rel_y + 40, ALLEGRO_ALIGN_CENTER, std::to_string(cappedamount[TEAM2]).c_str());
}

void ControlPoint::capture(Team cappedteam, std::shared_ptr<GameModeManager> gamemanager)
{
    cappingteam = cappedteam;
    cappedamount[cappedteam] = 0;
    cappedamount[1 - cappedteam] = 0;

    /*
    if (gamemanager->gamemode == Gamemode::CONTROL)
    {
        //
    }
    else if (gamemanager->gamemode == Gamemode::ASSAULT)
    {
        lock();
        // Unlock next CP
        // gameManager->controlPoints.next().unlock();
    }
    else if (gamemanager->gamemode == Gamemode::HYBRID)
    {
        lock();
        // Change things from Control to Escort

    }
    */

    // Send client about capture event if server
}

void ControlPoint::lock()
{
    locked = true;
}

void ControlPoint::unlock()
{
    locked = false;
}
