#include "ingameelements/heroes/mccree.h"
#include "datastructures.h"
#include "spriteloader.h"
#include "animation.h"
#include "gamestate.h"
#include "engine.h"
#include "ingameelements/projectiles/flashbang.h"

#include <memory>
#include <cmath>
#include <allegro5/allegro_primitives.h>

Mccree::Mccree(uint64_t id_, Gamestate *state, EntityPtr owner_) : Character(id_, state, owner_, constructparameters(id_, state, owner_)),
                rollanim("heroes/mccree/roll/"), flashbanganim("heroes/mccree/flashbang/"), rollcooldown(8), flashbangcooldown(10)
{
    rollanim.active(false);
    rollcooldown.active = false;
    flashbanganim.active(false);
    flashbangcooldown.active = false;
}

Mccree::~Mccree()
{
    //dtor
}

void Mccree::render(Renderer *renderer, Gamestate *state)
{
    Character::render(renderer, state);

    std::string mainsprite = getsprite(state, false);
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    // TODO Replace this with team check
    std::string outlinesprite;
    if (state->get<Player>(renderer->myself)->character == EntityPtr(id))
    {
        // Draw friendly outline
        outlinesprite = mainsprite+"_friend";
    }
    else
    {
        // Draw enemy outline
        outlinesprite = mainsprite+"_enemy";
    }
    ALLEGRO_BITMAP *outline = renderer->spriteloader.requestsprite(outlinesprite);
    int outlinespriteoffset_x = renderer->spriteloader.get_spriteoffset_x(outlinesprite);
    int outlinespriteoffset_y = renderer->spriteloader.get_spriteoffset_y(outlinesprite);

    al_set_target_bitmap(renderer->midground);
    if (isflipped)
    {
        // Flip horizontally
        al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, x-renderer->cam_x, y-renderer->cam_y, -1, 1, 0, 0);
        al_draw_scaled_rotated_bitmap(outline, outlinespriteoffset_x, outlinespriteoffset_y, x-renderer->cam_x, y-renderer->cam_y, -1, 1, 0, 0);
    }
    else
    {
        al_draw_bitmap(sprite, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 0);
        al_draw_bitmap(outline, x-outlinespriteoffset_x - renderer->cam_x, y-outlinespriteoffset_y - renderer->cam_y, 0);
    }

    if (flashbanganim.active())
    {
        std::string armsprite = flashbanganim.getframe();
        sprite = renderer->spriteloader.requestsprite(armsprite);
        spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(armsprite);
        spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(armsprite);
        if (isflipped)
        {
            // Flip horizontally
            al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, x-renderer->cam_x, y-renderer->cam_y, -1, 1, 0, 0);
        }
        else
        {
            al_draw_bitmap(sprite, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 0);
        }
    }

    state->get<Weapon>(weapon)->render(renderer, state);
}

void Mccree::drawhud(Renderer *renderer, Gamestate *state)
{
    Character::drawhud(renderer, state);

    double space = 3;
    float r[8];

    ALLEGRO_BITMAP *sprite;
    Rect spriterect = renderer->spriteloader.get_rect("ui/ingame/mccree/rolling");
    if (rollcooldown.active)
    {
        sprite = renderer->spriteloader.requestsprite("ui/ingame/mccree/rollingcooldown");
    }
    else
    {
        sprite = renderer->spriteloader.requestsprite("ui/ingame/mccree/rolling");
    }
    spriterect.x = renderer->WINDOW_WIDTH*6/7.0 - spriterect.w*2 - space;
    spriterect.y = renderer->WINDOW_HEIGHT*hudheight()-spriterect.h;
    al_draw_bitmap(sprite, spriterect.x, spriterect.y, 0);
    if (rollcooldown.active)
    {
        // Draw the fill-in
        r[0] = spriterect.x+17*rollcooldown.getpercent();
        r[1] = spriterect.y+2+34*(1-rollcooldown.getpercent());

        r[2] = spriterect.x;
        r[3] = spriterect.y+spriterect.h-2;

        r[4] = spriterect.x+39;
        r[5] = spriterect.y+spriterect.h-2;

        r[6] = spriterect.x+17*rollcooldown.getpercent()+39;
        r[7] = spriterect.y+2+34*(1-rollcooldown.getpercent());

        al_draw_filled_polygon(r, 4, al_premul_rgba_f(239/255.0, 179/255.0, 89/255.0, 0.5));

        al_draw_text(renderer->font10, al_map_rgb(255, 255, 255), spriterect.x+spriterect.w/2.0+2, spriterect.y+spriterect.h/2.0-al_get_font_line_height(renderer->font10)/2.0,
                        ALLEGRO_ALIGN_CENTER, std::to_string((int)std::ceil(rollcooldown.duration - rollcooldown.timer)).c_str());
    }

    if (flashbangcooldown.active)
    {
        sprite = renderer->spriteloader.requestsprite("ui/ingame/mccree/flashbangcooldown");
    }
    else
    {
        sprite = renderer->spriteloader.requestsprite("ui/ingame/mccree/flashbang");
    }
    spriterect.x = spriterect.x + spriterect.w+space;
    al_draw_bitmap(sprite, spriterect.x, spriterect.y, 0);
    if (flashbangcooldown.active)
    {
        // Draw the fill-in
        r[0] = spriterect.x+17*flashbangcooldown.getpercent();
        r[1] = spriterect.y+2+34*(1-flashbangcooldown.getpercent());

        r[2] = spriterect.x;
        r[3] = spriterect.y+spriterect.h-2;

        r[4] = spriterect.x+39;
        r[5] = spriterect.y+spriterect.h-2;

        r[6] = spriterect.x+17*flashbangcooldown.getpercent()+39;
        r[7] = spriterect.y+2+34*(1-flashbangcooldown.getpercent());

        al_draw_filled_polygon(r, 4, al_premul_rgba_f(239/255.0, 179/255.0, 89/255.0, 0.5));

        al_draw_text(renderer->font10, al_map_rgb(255, 255, 255), spriterect.x+spriterect.w/2.0+2, spriterect.y+spriterect.h/2.0-al_get_font_line_height(renderer->font10)/2.0,
                        ALLEGRO_ALIGN_CENTER, std::to_string((int)std::ceil(flashbangcooldown.duration - flashbangcooldown.timer)).c_str());
    }
}

void Mccree::midstep(Gamestate *state, double frametime)
{
    Character::midstep(state, frametime);

    rollanim.update(state, frametime);
    if (rollanim.active())
    {
        if (isflipped)
        {
            hspeed = -360;
        }
        else
        {
            hspeed = 360;
        }
    }
    flashbanganim.update(state, frametime);
    rollcooldown.update(state, frametime);
    flashbangcooldown.update(state, frametime);

    if (cangetinput(state))
    {
        if (held_keys.ABILITY_1 and not rollcooldown.active and onground(state) and state->engine->isserver)
        {
            useability1(state);
            state->sendbuffer->write<uint8_t>(ABILITY1_USED);
            state->sendbuffer->write<uint8_t>(state->findplayerid(owner));
        }
        if (held_keys.ABILITY_2 and not flashbangcooldown.active and state->engine->isserver)
        {
            useability2(state);
            state->sendbuffer->write<uint8_t>(ABILITY2_USED);
            state->sendbuffer->write<uint8_t>(state->findplayerid(owner));
        }
    }
}

void Mccree::useability1(Gamestate *state)
{
    if (held_keys.LEFT and not held_keys.RIGHT)
    {
        isflipped = true;
    }
    else if (held_keys.RIGHT and not held_keys.LEFT)
    {
        isflipped = false;
    }
    // Lets roll
    rollanim.reset();
    rollcooldown.reset();
    Peacemaker *w = reinterpret_cast<Peacemaker*>(getweapon(state));
    w->clip = w->getclipsize();
    w->reloadanim.active(0);
    w->isfthing = false;
    w->fthanim.active(false);
    vspeed = 0;
}

void Mccree::useability2(Gamestate *state)
{
    // Flashbang
    flashbanganim.reset();
    flashbangcooldown.reset();
    Flashbang *f = state->get<Flashbang>(state->make_entity<Flashbang>(state, owner));
    f->x = x;
    f->y = y;
    double dir = std::atan2(mouse_y-y, mouse_x-x);
    f->hspeed = std::cos(dir) * 300;
    f->vspeed = std::sin(dir) * 300;
}

Rect Mccree::getcollisionrect(Gamestate *state)
{
    if (crouchanim.active())
    {
        return state->engine->maskloader.get_rect_from_json(getcharacterfolder()+"crouch/").offset(x, y);
    }
    return getstandingcollisionrect(state);
}

Rect Mccree::getstandingcollisionrect(Gamestate *state)
{
    return state->engine->maskloader.get_rect_from_json(getcharacterfolder()).offset(x, y);
}

std::string Mccree::getsprite(Gamestate *state, bool mask)
{
    if (stunanim.active())
    {
        return stunanim.getframe();
    }
    if (rollanim.active())
    {
        return rollanim.getframe();
    }
    if (crouchanim.active())
    {
        return crouchanim.getframe();
    }
    if (std::fabs(hspeed) < 11.0)
    {
        return getcharacterfolder()+"idle/1";
    }
    return runanim.getframe();
}

CharacterChildParameters Mccree::constructparameters(uint64_t id_, Gamestate *state, EntityPtr owner_)
{
    CharacterChildParameters p;
    p.runpower = 1.8;
    p.weapon = state->make_entity<Peacemaker>(state, owner_);
    p.maxhp.normal = 200;
    p.maxhp.armor = 0;
    p.maxhp.shields = 0;
    p.characterfolder = "heroes/mccree/";
    return p;
}

Health Mccree::getmaxhp()
{
    Health maxhp;
    maxhp.normal = 200;
    maxhp.armor = 0;
    maxhp.shields = 0;
    return maxhp;
}
