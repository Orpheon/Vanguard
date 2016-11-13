#include "ingameelements/heroes/reinhardt.h"
#include "datastructures.h"
#include "spriteloader.h"
#include "animation.h"
#include "gamestate.h"
#include "engine.h"

#include <memory>
#include <cmath>
#include <allegro5/allegro_primitives.h>

Reinhardt::Reinhardt(uint64_t id_, Gamestate *state, EntityPtr owner_) : Character(id_, state, owner_, constructparameters(id_, state, owner_))
{

}

Reinhardt::~Reinhardt()
{
    //dtor
}

void Reinhardt::render(Renderer *renderer, Gamestate *state)
{
    Character::render(renderer, state);
    al_set_target_bitmap(renderer->midground);

    // Render weapon back first
    state->get<Hammer>(weapon)->renderbehind(renderer, state);

    std::string mainsprite;
    ALLEGRO_BITMAP *sprite;
    int spriteoffset_x;
    int spriteoffset_y;

    mainsprite = getsprite(state, false);
    sprite = renderer->spriteloader.requestsprite(mainsprite);
    spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    std::string outlinesprite = mainsprite+"_outline";
    ALLEGRO_BITMAP *outline = renderer->spriteloader.requestsprite(outlinesprite);
    int outlinespriteoffset_x = renderer->spriteloader.get_spriteoffset_x(outlinesprite);
    int outlinespriteoffset_y = renderer->spriteloader.get_spriteoffset_y(outlinesprite);
    ALLEGRO_COLOR outlinecolor;
    if (state->get<Player>(renderer->myself)->team == team)
    {
        // Draw friendly outline
        outlinecolor = al_map_rgb(0, 145, 181);
    }
    else
    {
        // Draw enemy outline
        outlinecolor = al_map_rgb(225, 17, 17);
    }

    if (isflipped)
    {
        // Flip horizontally
        al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, x-renderer->cam_x, y-renderer->cam_y, -1, 1, 0, 0);
        al_draw_tinted_scaled_rotated_bitmap(outline, outlinecolor, outlinespriteoffset_x, outlinespriteoffset_y, x-renderer->cam_x, y-renderer->cam_y, -1, 1, 0, 0);
    }
    else
    {
        al_draw_bitmap(sprite, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 0);
        al_draw_tinted_bitmap(outline, outlinecolor, x-outlinespriteoffset_x - renderer->cam_x, y-outlinespriteoffset_y - renderer->cam_y, 0);
    }

    state->get<Weapon>(weapon)->render(renderer, state);
}

void Reinhardt::drawhud(Renderer *renderer, Gamestate *state)
{
    Character::drawhud(renderer, state);

//    double space = 3;
//    float r[8];
//
//    ALLEGRO_BITMAP *sprite;
//    Rect spriterect = renderer->spriteloader.get_rect("ui/ingame/mccree/rolling");
//    if (rollcooldown.active)
//    {
//        sprite = renderer->spriteloader.requestsprite("ui/ingame/mccree/rollingcooldown");
//    }
//    else
//    {
//        sprite = renderer->spriteloader.requestsprite("ui/ingame/mccree/rolling");
//    }
//    spriterect.x = renderer->WINDOW_WIDTH*6/7.0 - spriterect.w*2 - space;
//    spriterect.y = renderer->WINDOW_HEIGHT*hudheight()-spriterect.h;
//    al_draw_bitmap(sprite, spriterect.x, spriterect.y, 0);
//    if (rollcooldown.active)
//    {
//        // Draw the fill-in
//        r[0] = spriterect.x+17*rollcooldown.getpercent();
//        r[1] = spriterect.y+2+34*(1-rollcooldown.getpercent());
//
//        r[2] = spriterect.x;
//        r[3] = spriterect.y+spriterect.h-2;
//
//        r[4] = spriterect.x+39;
//        r[5] = spriterect.y+spriterect.h-2;
//
//        r[6] = spriterect.x+17*rollcooldown.getpercent()+39;
//        r[7] = spriterect.y+2+34*(1-rollcooldown.getpercent());
//
//        al_draw_filled_polygon(r, 4, al_premul_rgba_f(239/255.0, 179/255.0, 89/255.0, 0.5));
//
//        al_draw_text(renderer->font10, al_map_rgb(255, 255, 255), spriterect.x+spriterect.w/2.0+2, spriterect.y+spriterect.h/2.0-al_get_font_line_height(renderer->font10)/2.0,
//                        ALLEGRO_ALIGN_CENTER, std::to_string((int)std::ceil(rollcooldown.duration - rollcooldown.timer)).c_str());
//    }
//
//    if (flashbangcooldown.active)
//    {
//        sprite = renderer->spriteloader.requestsprite("ui/ingame/mccree/flashbangcooldown");
//    }
//    else
//    {
//        sprite = renderer->spriteloader.requestsprite("ui/ingame/mccree/flashbang");
//    }
//    spriterect.x = spriterect.x + spriterect.w+space;
//    al_draw_bitmap(sprite, spriterect.x, spriterect.y, 0);
//    if (flashbangcooldown.active)
//    {
//        // Draw the fill-in
//        r[0] = spriterect.x+17*flashbangcooldown.getpercent();
//        r[1] = spriterect.y+2+34*(1-flashbangcooldown.getpercent());
//
//        r[2] = spriterect.x;
//        r[3] = spriterect.y+spriterect.h-2;
//
//        r[4] = spriterect.x+39;
//        r[5] = spriterect.y+spriterect.h-2;
//
//        r[6] = spriterect.x+17*flashbangcooldown.getpercent()+39;
//        r[7] = spriterect.y+2+34*(1-flashbangcooldown.getpercent());
//
//        al_draw_filled_polygon(r, 4, al_premul_rgba_f(239/255.0, 179/255.0, 89/255.0, 0.5));
//
//        al_draw_text(renderer->font10, al_map_rgb(255, 255, 255), spriterect.x+spriterect.w/2.0+2, spriterect.y+spriterect.h/2.0-al_get_font_line_height(renderer->font10)/2.0,
//                        ALLEGRO_ALIGN_CENTER, std::to_string((int)std::ceil(flashbangcooldown.duration - flashbangcooldown.timer)).c_str());
//    }
}

void Reinhardt::midstep(Gamestate *state, double frametime)
{
    Character::midstep(state, frametime);

    if (cangetinput(state))
    {
        if (heldkeys.ABILITY_1 and onground(state) and state->engine->isserver)
        {
            useability1(state);
            state->sendbuffer->write<uint8_t>(ABILITY1_USED);
            state->sendbuffer->write<uint8_t>(state->findplayerid(owner));
        }
        if (heldkeys.ABILITY_2 and state->engine->isserver)
        {
            useability2(state);
            state->sendbuffer->write<uint8_t>(ABILITY2_USED);
            state->sendbuffer->write<uint8_t>(state->findplayerid(owner));
        }
    }
}

void Reinhardt::useability1(Gamestate *state)
{

}

void Reinhardt::useability2(Gamestate *state)
{

}

Rect Reinhardt::getcollisionrect(Gamestate *state)
{
    if (crouchanim.active())
    {
        return state->engine->maskloader.get_rect_from_json(getcharacterfolder()+"crouch/").offset(x, y);
    }
    return getstandingcollisionrect(state);
}

Rect Reinhardt::getstandingcollisionrect(Gamestate *state)
{
    return state->engine->maskloader.get_rect_from_json(getcharacterfolder()).offset(x, y);
}

std::string Reinhardt::getsprite(Gamestate *state, bool mask)
{
    if (stunanim.active())
    {
        return stunanim.getframepath();
    }
    if (crouchanim.active())
    {
        return crouchanim.getframepath();
    }
    if (not ongroundsmooth.active)
    {
        if (vspeed > 100)
        {
            return getcharacterfolder()+"falling/1";
        }
        else
        {
            return getcharacterfolder()+"jump/1";
        }
    }
    if (std::fabs(hspeed) < 11.0 and not heldkeys.LEFT and not heldkeys.RIGHT)
    {
        return getcharacterfolder()+"idle/1";
    }
    return runanim.getframepath();
}

CharacterChildParameters Reinhardt::constructparameters(uint64_t id_, Gamestate *state, EntityPtr owner_)
{
    CharacterChildParameters p;
    p.runpower = 1.8;
    p.weapon = state->make_entity<Hammer>(state, owner_);
    p.maxhp.normal = 300;
    p.maxhp.armor = 200;
    p.maxhp.shields = 0;
    p.characterfolder = "heroes/reinhardt/";
    return p;
}

Health Reinhardt::getmaxhp()
{
    Health maxhp;
    maxhp.normal = 300;
    maxhp.armor = 200;
    maxhp.shields = 0;
    return maxhp;
}
