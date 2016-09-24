#include "mccree.h"
#include "datastructures.h"
#include "spriteloader.h"
#include "animation.h"
#include "gamestate.h"

#include <memory>
#include <cmath>

Mccree::Mccree(Gamestate *state, PlayerPtr owner_) : Character(state, owner_, constructCharacterChildParameters())
{
    ;
}

Mccree::~Mccree()
{
    //dtor
}

void Mccree::render(Renderer *renderer)
{
    std::string mainsprite = walkanim.get_frame();
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


CharacterChildParameters Mccree::constructCharacterChildParameters()
{
    CharacterChildParameters c;
    c.walkanimpath = "heroes/mccree/walking/";
    return c;
}
