#include "mapelements/controlpoint.h"
#include "gamestate.h"
#include "renderer.h"
#include "colorpalette.h"

#include "ingameelements/gamemodes/kothmanager.h"

#include <allegro5/allegro_primitives.h>

void ControlPoint::init(uint64_t id_, Rect area_, Team owningteam_, std::function<void(Gamestate &state, Team winningteam)> capturefunc_)
{
    Entity::init(id_);
    area = area_;
    capturefunc = capturefunc_;
    capamount.init(MAXCAP, std::bind(&ControlPoint::capture, this, std::placeholders::_1));
    capfalloff.init(5);
    owningteam = owningteam_;
    cappingteam = NO_TEAM;
}

void ControlPoint::beginstep(Gamestate &state, double frametime)
{
    bool capping = false;
    int nplayerscapping = 0;
    bool capblocked = false;

    for (auto &pptr : state.playerlist)
    {
        Player &p = state.get<Player>(pptr);
        if (state.exists(p.character))
        {
            Character &c = p.getcharacter(state);
            if (isinside(c.x, c.y))
            {
                if (p.team == cappingteam or (capamount.getpercent() == 0 and p.team != owningteam))
                {
                    capping = true;
                    ++nplayerscapping;
                    cappingteam = p.team;
                }
                else
                {
                    capblocked = true;
                }
            }
        }
    }

    capfalloff.update(state, frametime);

    if (capping)
    {
        if (capblocked)
        {
            // TODO: "Contesting" HUD
        }
        else
        {
            capamount.update(state, CAPRATE * frametime * std::min(nplayerscapping, 3));
        }
        capfalloff.reset();
    }

    if (not capfalloff.active)
    {
        // No-one of the capping team has touched the point in a while,
        // point capture should degenerate to the latest third
        if (owningteam != NO_TEAM)
        {
            capamount.timer = std::max(capamount.timer - CAPDEGEN*frametime, std::floor(capamount.timer * 3.0/MAXCAP) * MAXCAP/3.0);
        }
        else
        {
            capamount.timer = std::max(capamount.timer - CAPDEGEN*frametime, 0.0);
        }
    }
}

void ControlPoint::render(Renderer &renderer, Gamestate &state)
{
    // Find the center of CP area
    double rel_x = (area.x + area.w/2 - renderer.cam_x)*renderer.zoom;
    double rel_y = (area.y + area.h/2 - renderer.cam_y)*renderer.zoom;
    Color owningcolor;
    Color cappingcolor;
    
    if (owningteam == NO_TEAM)
    {
        owningcolor = Color::CP;
    }
    else if (state.get<Player>(renderer.myself).team == owningteam)
    {
        owningcolor = Color::ALLY;
    }
    else
    {
        owningcolor = Color::ENEMY;
    }

    if (cappingteam == NO_TEAM)
    {
        cappingcolor = Color::CP;
    }
    else if (state.get<Player>(renderer.myself).team == cappingteam)
    {
        cappingcolor = Color::ALLY;
    }
    else
    {
        cappingcolor = Color::ENEMY;
    }

    ALLEGRO_COLOR owningcolorfront = ColorPalette::premul(owningcolor, 200);
    ALLEGRO_COLOR owningcolormid = ColorPalette::premul(owningcolor, 160);
    ALLEGRO_COLOR owningcolorback = ColorPalette::premul(owningcolor, 80);

    // Draw CP Area
    al_set_target_bitmap(renderer.foreground);
    al_draw_line(rel_x - (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        rel_x + (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        owningcolorfront, 5);
    al_draw_line(rel_x - (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        rel_x + (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        owningcolormid, 3);

    al_set_target_bitmap(renderer.background);
    al_draw_line(rel_x - (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        rel_x - (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 20)*renderer.zoom,
        owningcolormid, 3);
    al_draw_line(rel_x + (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 10)*renderer.zoom,
        rel_x + (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 20)*renderer.zoom,
        owningcolormid, 3);

    al_draw_line(rel_x - (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 20)*renderer.zoom,
        rel_x + (area.w / 2)*renderer.zoom, rel_y + ((area.h / 2) - 20)*renderer.zoom,
        owningcolorback, 3);

    // Draw CP Bubble 
    al_draw_filled_circle(rel_x, rel_y, 30.0, owningcolorfront);
    al_draw_circle(rel_x, rel_y, 30.0, owningcolorfront, 5);
    if (capamount.getpercent() > 0)
    {
        ALLEGRO_COLOR cc = ColorPalette::get(cappingcolor);
        al_draw_arc(rel_x, rel_y, 31.0, -PI/2.0, 2*PI*capamount.getpercent(), cc, 5);
    }
    al_draw_text(renderer.font20, ColorPalette::get(Color::WHITE), rel_x + 5, rel_y - 20, ALLEGRO_ALIGN_CENTER, "A");
}

void ControlPoint::interpolate(Entity &prev_entity, Entity &next_entity, double alpha)
{
    ControlPoint &prev_e = static_cast<ControlPoint&>(prev_entity);
    ControlPoint &next_e = static_cast<ControlPoint&>(next_entity);

    if (alpha < 0.5)
    {
        owningteam = prev_e.owningteam;
        cappingteam = prev_e.cappingteam;
    }
    else
    {
        owningteam = next_e.owningteam;
        cappingteam = next_e.cappingteam;
    }

    capamount.interpolate(prev_e.capamount, next_e.capamount, alpha);
    capfalloff.interpolate(prev_e.capfalloff, next_e.capfalloff, alpha);
}

void ControlPoint::serializefull(WriteBuffer &buffer, Gamestate &state)
{
    buffer.write<double>(capamount.timer);
    buffer.write<double>(capfalloff.timer);
    buffer.write<char>(owningteam);
    buffer.write<char>(cappingteam);
}

void ControlPoint::deserializefull(ReadBuffer &buffer, Gamestate &state)
{
    capamount.timer = buffer.read<double>();
    Global::logging().print(__FILE__, __LINE__, "Client received capamount %f", capamount.timer);
    capfalloff.timer = buffer.read<double>();
    owningteam = static_cast<Team>(buffer.read<char>());
    cappingteam = static_cast<Team>(buffer.read<char>());
}