#include "mccree.h"
#include "datastructures.h"
#include "spriteloader.h"
#include "animation.h"
#include "gamestate.h"
#include "engine.h"

#include <memory>
#include <cmath>

Mccree::Mccree(Gamestate *state, EntityPtr owner_) : Character(state, owner_, constructparameters())
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
    if (isflipped)
    {
        // Flip horizontally
        al_draw_scaled_rotated_bitmap(sprite, spriteoffset_x, spriteoffset_y, x-renderer->cam_x, y-renderer->cam_y, -1, 1, 0, 0);
    }
    else
    {
        al_draw_bitmap(sprite, x-spriteoffset_x - renderer->cam_x, y-spriteoffset_y - renderer->cam_y, 0);
    }

    state->get<Weapon>(weapon)->render(renderer, state);
}

/*void Mccree::midstep(Gamestate *state, double frametime)
{
    Character::midstep(state, frametime);

    if (rolling)
    {
        // TODO
    }
    else
    {
        isflipped = mouse_x < 0;
        if (pressed_keys.ABILITY_1)
        {
            // Lets roll
            rolling = true;
            rolling_spriteindex = 0;
            hspeed = std::copysign(300.0, mouse_x);
        }
    }
}
*/

Rect Mccree::getcollisionrect(Gamestate *state)
{
    if (crouched)
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
    if (crouched)
    {
        return "heroes/mccree/crouch/1.png";
    }
    if (not onground(state))
    {
        return "heroes/mccree/jump/1.png";
    }
    if (std::fabs(hspeed) < 11.0)
    {
        return "heroes/mccree/idle/1.png";
    }
    return runanim.get_frame();
}

CharacterChildParameters Mccree::constructparameters()
{
    CharacterChildParameters p;
    p.runanimfolder = "heroes/mccree/run/";
    p.runpower = 1.8;
    p.weapon = state->make_entity<Peacemaker>(state, EntityPtr(id));
    return p;
}
