#include "mccree.h"
#include "datastructures.h"
#include "spriteloader.h"
#include "animation.h"
#include "gamestate.h"
#include "engine.h"

#include <memory>
#include <cmath>

Mccree::Mccree(uint64_t id_, Gamestate *state, EntityPtr owner_) : Character(id_, state, owner_, constructparameters(id_, state)), animstate_()
{
    ;
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

    if (cangetinput(state))
    {
        if (pressed_keys.ABILITY_1 and not animstate()->crouchanim.active())
        {
            // Lets roll
            animstate()->rolling.reset();
            animstate()->rolling.active(true);
        }
        if (pressed_keys.ABILITY_2)
        {
            // Flashbang
            animstate()->flashbang.reset();
            animstate()->flashbang.active(true);
        }
    }
}


Rect Mccree::getcollisionrect(Gamestate *state)
{
    if (animstate()->crouchanim.active())
    {
        return state->engine->maskloader.get_rect("heroes/mccree/crouch/").offset(x, y);
    }
    return getstandingcollisionrect(state);
}

Rect Mccree::getstandingcollisionrect(Gamestate *state)
{
    return state->engine->maskloader.get_rect("heroes/mccree/").offset(x, y);
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
    return p;
}
