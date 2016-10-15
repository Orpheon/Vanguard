#include "ingameelements/heroes/mccree.h"
#include "datastructures.h"
#include "spriteloader.h"
#include "animation.h"
#include "gamestate.h"
#include "engine.h"
#include "ingameelements/projectiles/flashbang.h"

#include <memory>
#include <cmath>

Mccree::Mccree(uint64_t id_, Gamestate *state, EntityPtr owner_) : Character(id_, state, owner_, constructparameters(id_, state)), rollcooldown(8), flashbangcooldown(10), animstate_()
{
    rollcooldown.active = false;
    flashbangcooldown.active = false;
}

Mccree::~Mccree()
{
    //dtor
}

void Mccree::render(Renderer *renderer, Gamestate *state)
{
    std::string mainsprite = getsprite(state, false);
    ALLEGRO_BITMAP *sprite = renderer->spriteloader.request_sprite(mainsprite);
    int spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(mainsprite);
    int spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(mainsprite);

    al_set_target_bitmap(renderer->midground);
    if (animstate()->isflipped)
    {
        // Flip horizontally
        al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, x-renderer->cam_x, y-renderer->cam_y, -1, 1, 0, 0);
    }
    else
    {
        al_draw_bitmap(sprite, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 0);
    }

    if (animstate()->flashbang.active())
    {
        std::string armsprite = animstate()->flashbang.getframe();
        sprite = renderer->spriteloader.request_sprite(armsprite);
        spriteoffset_x = renderer->spriteloader.get_spriteoffset_x(armsprite);
        spriteoffset_y = renderer->spriteloader.get_spriteoffset_y(armsprite);
        if (animstate()->isflipped)
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

void Mccree::midstep(Gamestate *state, double frametime)
{
    Character::midstep(state, frametime);

    animstate()->rolling.update(state, frametime);
    if (animstate()->rolling.active())
    {
        if (animstate()->isflipped)
        {
            hspeed = -300;
        }
        else
        {
            hspeed = 300;
        }
    }
    animstate()->flashbang.update(state, frametime);
    rollcooldown.update(state, frametime);
    flashbangcooldown.update(state, frametime);

    if (cangetinput(state))
    {
        if (held_keys.ABILITY_1 and not rollcooldown.active and state->engine->isserver)
        {
            // Lets roll
            if (lastdirectionpressed == LEFT)
            {
                animstate()->isflipped = true;
            }
            else if (lastdirectionpressed == RIGHT)
            {
                animstate()->isflipped = false;
            }
            animstate()->rolling.reset();
            rollcooldown.reset();
            Weapon *p = state->get<Peacemaker>(weapon);
            p->clip = p->getclipsize();
            p->reloadanim.active(0);
        }
        if (held_keys.ABILITY_2 and not flashbangcooldown.active and state->engine->isserver)
        {
            // Flashbang
            animstate()->flashbang.reset();
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
    if (animstate()->crouchanim.active())
    {
        return state->engine->maskloader.get_rect_from_json("heroes/mccree/crouch/").offset(x, y);
    }
    return getstandingcollisionrect(state);
}

Rect Mccree::getstandingcollisionrect(Gamestate *state)
{
    return state->engine->maskloader.get_rect_from_json("heroes/mccree/").offset(x, y);
}

std::string Mccree::getsprite(Gamestate *state, bool mask)
{
    if (animstate()->rolling.active())
    {
        return animstate()->rolling.getframe();
    }
    if (animstate()->crouchanim.active())
    {
        return animstate()->crouchanim.getframe();
    }
    if (std::fabs(hspeed) < 11.0)
    {
        return "heroes/mccree/idle/1.png";
    }
    return animstate()->runanim.getframe();
}

CharacterChildParameters Mccree::constructparameters(uint64_t id_, Gamestate *state)
{
    CharacterChildParameters p;
    p.runpower = 1.8;
    p.weapon = state->make_entity<Peacemaker>(state, EntityPtr(id_));
    p.maxhp = 200;
    return p;
}
