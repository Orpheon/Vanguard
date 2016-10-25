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

Mccree::Mccree(uint64_t id_, Gamestate *state, EntityPtr owner_) : Character(id_, state, owner_, constructparameters(id_, state)),
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
    std::string mainsprite = getsprite(state, false);
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.requestsprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->midground);
    if (isflipped)
    {
        // Flip horizontally
        al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, x-renderer->cam_x, y-renderer->cam_y, -1, 1, 0, 0);
    }
    else
    {
        al_draw_bitmap(sprite, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 0);
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


    // --------------- HEALTHBAR ---------------
    al_set_target_bitmap(renderer->surfaceground);
    double healthalpha = 1.0;
    double lack_healthalpha = 0.2;

    // Normal, armor, shields
    ALLEGRO_COLOR EXISTING_HEALTH[] = { al_premul_rgba_f(225/255.0, 225/255.0, 225/255.0, healthalpha),
                                        al_premul_rgba_f(237/255.0, 223/255.0, 132/255.0, healthalpha),
                                        al_premul_rgba_f(101/255.0, 206/255.0, 240/255.0, healthalpha)};
    ALLEGRO_COLOR LACKING_HEALTH[] = {  al_premul_rgba_f(225/255.0, 225/255.0, 225/255.0, lack_healthalpha),
                                        al_premul_rgba_f(237/255.0, 223/255.0, 132/255.0, lack_healthalpha),
                                        al_premul_rgba_f(101/255.0, 206/255.0, 240/255.0, lack_healthalpha)};

    Health maxhp = getmaxhp();
    double x_;
    float r[8]; // Array used to pass the polygon data for the actual drawing

    // Parameters
    double totalwidth = 30;
    int height = 5;
    int space = 1;
    double slant = 0.3;
    double y_ = y - renderer->spriteloader.get_spriteoffset_y(mainsprite) - renderer->cam_y - 15;

    int nrects = std::ceil(maxhp.total()/25.0);
    double width = totalwidth/nrects;
    Rect box = renderer->spriteloader.get_rect(mainsprite);
    double start_x = x - renderer->spriteloader.get_spriteoffset_x(mainsprite) + box.w/2.0 - renderer->cam_x - width*(nrects/2.0) - space*((nrects-1)/2.0);

    // Draw first normal health, then armor, then shields
    for (int healthtype=0; healthtype<3; ++healthtype)
    {
        double hppercent = 1.0;
        if (healthtype == 0)
        {
            nrects = std::ceil(maxhp.normal/25.0);
            if (nrects == 0)
            {
                continue;
            }
            hppercent = hp.normal/maxhp.normal;
        }
        else if (healthtype == 1)
        {
            nrects = std::ceil(maxhp.armor/25.0);
            if (nrects == 0)
            {
                continue;
            }
            hppercent = hp.armor/maxhp.armor;
        }
        else if (healthtype == 2)
        {
            nrects = std::ceil(maxhp.shields/25.0);
            if (nrects == 0)
            {
                continue;
            }
            hppercent = hp.shields/maxhp.shields;
        }

        // Full existing health boxes
        for (int i=0; i<std::floor(nrects*hppercent); ++i)
        {
            x_ = start_x + i*width + (i-1)*space;
            r[0] = x_+height*slant;
            r[1] = y_;

            r[2] = x_;
            r[3] = y_+height;

            r[4] = x_+width;
            r[5] = y_+height;

            r[6] = x_+width+height*slant;
            r[7] = y_;
            al_draw_filled_polygon(r, 4, EXISTING_HEALTH[healthtype]);
        }
        // Interface between existing and lacking health
        double leftover = nrects*hppercent - std::floor(nrects*hppercent);
        if (leftover > 0.0)
        {
            // Draw the half-rectangle
            x_ = start_x + std::floor(nrects*hppercent)*width + (std::floor(nrects*hppercent)-1)*space;
            r[0] = x_+height*slant;
            r[1] = y_;

            r[2] = x_;
            r[3] = y_+height;

            r[4] = x_+width*leftover;
            r[5] = y_+height;

            r[6] = x_+width*leftover+height*slant;
            r[7] = y_;
            al_draw_filled_polygon(r, 4, EXISTING_HEALTH[healthtype]);

            // Draw the other half rectangle
            r[0] = x_+width*leftover+height*slant;
            r[1] = y_;

            r[2] = x_+width*leftover;
            r[3] = y_+height;

            r[4] = x_+width;
            r[5] = y_+height;

            r[6] = x_+width+height*slant;
            r[7] = y_;
            al_draw_filled_polygon(r, 4, LACKING_HEALTH[healthtype]);
        }
        // Full lacking health boxes
        for (int i=std::ceil(nrects*hppercent); i<nrects; ++i)
        {
            x_ = start_x + i*width + (i-1)*space;
            r[0] = x_+height*slant;
            r[1] = y_;

            r[2] = x_;
            r[3] = y_+height;

            r[4] = x_+width;
            r[5] = y_+height;

            r[6] = x_+width+height*slant;
            r[7] = y_;
            al_draw_filled_polygon(r, 4, LACKING_HEALTH[healthtype]);
        }
//        // Outline
//        for (int i=0; i<nrects; ++i)
//        {
//            x_ = start_x + i*width + (i-1)*space;
//            r[0] = x_+height*slant;
//            r[1] = y_;
//
//            r[2] = x_;
//            r[3] = y_+height;
//
//            r[4] = x_+width;
//            r[5] = y_+height;
//
//            r[6] = x_+width+height*slant;
//            r[7] = y_;
//            al_draw_polygon(r, 4, ALLEGRO_LINE_JOIN_ROUND, EXISTING_HEALTH[healthtype], 0, 0);
//        }
        // Offset starting position for the next health
        start_x += nrects*(width + space);
    }

    // --------------- /HEALTHBAR ---------------
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
        Peacemaker *p = state->get<Peacemaker>(weapon);
        if (held_keys.ABILITY_1 and not rollcooldown.active and onground(state) and state->engine->isserver)
        {
            // Lets roll
            if (lastdirectionpressed == LEFT)
            {
                isflipped = true;
            }
            else if (lastdirectionpressed == RIGHT)
            {
                isflipped = false;
            }
            rollanim.reset();
            rollcooldown.reset();
            p->clip = p->getclipsize();
            p->reloadanim.active(0);
            Peacemaker *w = reinterpret_cast<Peacemaker*>(getweapon(state));
            w->isfthing = false;
            w->fthanim.active(false);
        }
        if (held_keys.ABILITY_2 and not flashbangcooldown.active and state->engine->isserver)
        {
            // Flashbang
            flashbanganim.reset();
            flashbangcooldown.reset();
            Flashbang *f = state->get<Flashbang>(state->make_entity<Flashbang>(state, EntityPtr(id)));
            f->x = x;
            f->y = y;
            double dir = std::atan2(mouse_y, mouse_x);
            f->hspeed = std::cos(dir) * 300;
            f->vspeed = std::sin(dir) * 300;
        }
    }
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
        return getcharacterfolder()+"idle/1.png";
    }
    return runanim.getframe();
}

CharacterChildParameters Mccree::constructparameters(uint64_t id_, Gamestate *state)
{
    CharacterChildParameters p;
    p.runpower = 1.8;
    p.weapon = state->make_entity<Peacemaker>(state, EntityPtr(id_));
    p.maxhp.normal = 200;
    p.maxhp.armor = 100;
    p.maxhp.shields = 50;
    p.characterfolder = "heroes/mccree/";
    return p;
}

Health Mccree::getmaxhp()
{
    Health maxhp;
    maxhp.normal = 200;
    maxhp.armor = 100;
    maxhp.shields = 50;
    return maxhp;
}
