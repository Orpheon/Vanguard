
#include <gamestate.h>
#include <engine.h>
#include "ingameelements/weapons/sonicamp.h"
#include "ingameelements/heroes/lucio.h"

void Sonicamp::init(uint64_t id_, Gamestate &state, EntityPtr owner_)
{
    Clipweapon::init(id_, state, owner_);
}

void Sonicamp::renderbehind(Renderer &renderer, Gamestate &state)
{
    std::string mainsprite;
    Lucio &c = state.get<Lucio&>(state.get<Player>(owner).character);
    std::string charactersprite = c.currentsprite(state, false);
    if (charactersprite.find("/run/") != std::string::npos)
    {
        mainsprite = herofolder() + "runbackarm/" + std::to_string(c.runanim.getframe());
    }
    else
    {
        mainsprite = herofolder()+"backarm/1";
    }

    ALLEGRO_BITMAP *sprite = renderer.spriteloader.requestsprite(mainsprite);
    double spriteoffset_x = renderer.spriteloader.get_spriteoffset_x(mainsprite)*renderer.zoom;
    double spriteoffset_y = renderer.spriteloader.get_spriteoffset_y(mainsprite)*renderer.zoom;
    double rel_x = (x - renderer.cam_x)*renderer.zoom;
    double rel_y = (y - renderer.cam_y)*renderer.zoom;
    double attachpt_x = getbackattachpoint_x(state)*renderer.zoom;
    double attachpt_y = getbackattachpoint_y(state)*renderer.zoom;

    ALLEGRO_BITMAP *outline = renderer.spriteloader.requestspriteoutline(mainsprite);
    ALLEGRO_COLOR outlinecolor = al_map_rgb(225, 17, 17);

    al_set_target_bitmap(renderer.midground);
    if (c.weaponvisible(state))
    {
        if (c.isflipped)
        {
            al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x-attachpt_x, rel_y-attachpt_y,
                                          -1, 1, 0, 0);
            if (state.get<Player>(renderer.myself).team != team)
            {
                // Draw enemy outline
                al_draw_tinted_scaled_rotated_bitmap(outline, outlinecolor, attachpt_x+spriteoffset_x,
                                                     attachpt_y+spriteoffset_y, rel_x, rel_y, -1, 1, 0, 0);
            }
        }
        else
        {
            al_draw_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, rel_x-attachpt_x, rel_y-attachpt_y, 0, 0);
            if (state.get<Player>(renderer.myself).team != team)
            {
                // Draw enemy outline
                al_draw_tinted_rotated_bitmap(outline, outlinecolor, attachpt_x+spriteoffset_x,
                                              attachpt_y+spriteoffset_y, rel_x, rel_y, 0, 0);
            }
        }
    }
}

void Sonicamp::render(Renderer &renderer, Gamestate &state)
{
    // TODO
    ;
}

void Sonicamp::fireprimary(Gamestate &state)
{
    Global::logging().print(__FILE__, __LINE__, "Fired primary");
}

void Sonicamp::firesecondary(Gamestate &state)
{
    Global::logging().print(__FILE__, __LINE__, "Fired secondary");
}

void Sonicamp::wantfireprimary(Gamestate &state)
{
    if (clip > 0 and not firinganim.active() and not reloadanim.active() and state.engine.isserver)
    {
        fireprimary(state);
        state.engine.sendbuffer.write<uint8_t>(PRIMARY_FIRED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
    }
}

void Sonicamp::wantfiresecondary(Gamestate &state)
{
    if (clip > 0 and not firinganim.active() and not reloadanim.active() and state.engine.isserver)
    {
        firesecondary(state);
        state.engine.sendbuffer.write<uint8_t>(SECONDARY_FIRED);
        state.engine.sendbuffer.write<uint8_t>(state.findplayerid(owner));
    }
}

double Sonicamp::getbackattachpoint_x(Gamestate &state)
{
    return 0 * (state.get<Player&>(owner).getcharacter(state).isflipped ? -1:1);
}

double Sonicamp::getbackattachpoint_y(Gamestate &state)
{
    return 0;
}